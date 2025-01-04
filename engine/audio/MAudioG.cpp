/*********************************************************************
* \file   MAudioG.cpp
* \brief
*  _____ _____ _____
* |     |     |   __| MAudioG.h
* | | | |  |  |  |_ | Ver4.00
* |_|_|_|__|__|_____| 2024/09/23
*
* \author Harukichimaru
* \date   January 2025
* \note
*********************************************************************/

#define NOMINMAX
#include "MAudioG.h"
#include <mmdeviceapi.h>
#include <Functiondiscoverykeys_devpkey.h>

///=============================================================================
///						シングルトンインスタンスの取得
MAudioG* MAudioG::GetInstance() {
	static MAudioG instance;
	return &instance;
}

///=============================================================================
///						デバイスの取得
void MAudioG::GetAudioDevices() {
	// 省略（既存の実装）
}

///=============================================================================
///						初期化
void MAudioG::Initialize(const std::string& directoryPath, const std::wstring& deviceId) {
	HRESULT result;

	this->directoryPath_ = directoryPath;

	result = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	if (FAILED(result)) {
		std::cerr << "Failed to initialize XAudio2: " << std::hex << result << std::endl;
		return; // 初期化に失敗したら終了
	}

	result = xAudio2_->CreateMasteringVoice(&masterVoice_, XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE, 0, deviceId.empty() ? nullptr : deviceId.c_str(), nullptr);
	if (FAILED(result)) {
		std::cerr << "Failed to create mastering voice: " << std::hex << result << std::endl;
		return; // 初期化に失敗したら終了
	}

	waveSamplingRate = 44100.0f;
}

///=============================================================================
///						終了処理
void MAudioG::Finalize() {
	//========================================
	// 再生中のボイスを停止
	for (auto& [filename, voice] : voiceMap_) {
		if (voice.sourceVoice) {
			voice.sourceVoice->DestroyVoice();
		}
	}
	voiceMap_.clear();

	//========================================
	// ロードされたサウンドデータをクリア
	soundDataMap_.clear();

	//========================================
	// マスターボイスを破棄
	if (masterVoice_) {
		masterVoice_->DestroyVoice();
		masterVoice_ = nullptr;
	}
	//========================================
	// XAudio2のインスタンスをリセット
	xAudio2_.Reset();
}

///=============================================================================
///						サウンドのロード
void MAudioG::LoadWav(const std::string& filename) {
	// 既にロードされている場合は何もしない
	if (soundDataMap_.find(filename) != soundDataMap_.end()) {
		return;
	}

	//========================================
	// ファイルをバイナリモードで開く
	std::ifstream file(directoryPath_ + filename, std::ios_base::binary);
	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << filename << std::endl;
		return;
	}

	//========================================
	// RIFFヘッダーを読み込む
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0 || strncmp(riff.type, "WAVE", 4) != 0) {
		std::cerr << "Invalid RIFF or WAVE header in file: " << filename << std::endl;
		file.close();
		return;
	}

	//========================================
	// フォーマットチャンクを読み込む
	FormatChunk format = {};
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		std::cerr << "Invalid fmt chunk in file: " << filename << std::endl;
		file.close();
		return;
	}
	file.read((char*)&format.fmt, format.chunk.size);

	//========================================
	// データチャンクを読み込む
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	while (strncmp(data.id, "data", 4) != 0) {
		// 他のチャンクをスキップ
		file.seekg(data.size, std::ios_base::cur);
		file.read((char*)&data, sizeof(data));
	}

	//========================================
	// 音声データのバッファを読み込む
	std::vector<uint8_t> buffer(data.size);
	file.read(reinterpret_cast<char*>(buffer.data()), data.size);
	file.close();

	//========================================
	// サウンドデータを設定
	SoundData soundData = {};
	soundData.wfex = format.fmt;
	soundData.buffer = std::move(buffer);
	soundData.name = filename;

	//========================================
	// サウンドデータをマップに保存
	soundDataMap_[filename] = std::move(soundData);
}

///=============================================================================
///						サウンドデータのアンロード
void MAudioG::Unload(SoundData* soundData) {
	soundData->buffer.clear();
	soundData->name.clear();
}

///=============================================================================
///						サウンドを再生
void MAudioG::PlayWav(const std::string& filename, bool loopFlag, float volume, float maxPlaySpeed) {
	// サウンドデータがロードされていなければロードする
	if (soundDataMap_.find(filename) == soundDataMap_.end()) {
		LoadWav(filename);
	}

	auto& soundData = soundDataMap_[filename];

	// 既に再生中の場合は一旦停止
	StopWav(filename);

	// ソースボイスを作成
	IXAudio2SourceVoice* sourceVoice = nullptr;
	HRESULT result = xAudio2_->CreateSourceVoice(&sourceVoice, &soundData.wfex, XAUDIO2_VOICE_USEFILTER, maxPlaySpeed, &voiceCallback_);
	if (FAILED(result)) {
		std::cerr << "Failed to create source voice: " << std::hex << result << std::endl;
		return;
	}

	// バッファを設定
	XAUDIO2_BUFFER buf = {};
	buf.pAudioData = soundData.buffer.data();
	buf.AudioBytes = (UINT32)soundData.buffer.size();
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.LoopCount = loopFlag ? XAUDIO2_LOOP_INFINITE : 0;

	// ソースボイスにバッファを送信
	result = sourceVoice->SubmitSourceBuffer(&buf);
	if (FAILED(result)) {
		std::cerr << "Failed to submit source buffer: " << std::hex << result << std::endl;
		sourceVoice->DestroyVoice();
		return;
	}

	// 再生開始
	result = sourceVoice->Start(0);
	if (FAILED(result)) {
		std::cerr << "Failed to start playback: " << std::hex << result << std::endl;
		sourceVoice->DestroyVoice();
		return;
	}

	// ボイスを作成し、マップに追加
	Voice voice = {};
	voice.sourceVoice = sourceVoice;
	voice.sourceVoice->SetVolume(volume);
	voice.oldVolume = volume;
	voice.oldSpeed = 1.0f;

	std::lock_guard<std::mutex> lock(voiceMutex_);
	voiceMap_[filename] = std::move(voice);
}

///--------------------------------------------------------------
///						 サウンドの停止
void MAudioG::StopWav(const std::string& filename) {
	std::lock_guard<std::mutex> lock(voiceMutex_);
	auto it = voiceMap_.find(filename);
	if (it != voiceMap_.end()) {
		it->second.sourceVoice->Stop();
		it->second.sourceVoice->DestroyVoice();
		voiceMap_.erase(it);
	}
}

///--------------------------------------------------------------
///						 再生中かどうかを確認
bool MAudioG::IsWavPlaying(const std::string& filename) {
	std::lock_guard<std::mutex> lock(voiceMutex_);
	auto it = voiceMap_.find(filename);
	if (it != voiceMap_.end()) {
		XAUDIO2_VOICE_STATE state;
		it->second.sourceVoice->GetState(&state);
		return state.BuffersQueued > 0;
	}
	return false;
}

///--------------------------------------------------------------
///						 再生を一時停止
void MAudioG::PauseWav(const std::string& filename) {
	std::lock_guard<std::mutex> lock(voiceMutex_);
	auto it = voiceMap_.find(filename);
	if (it != voiceMap_.end()) {
		it->second.sourceVoice->Stop();
	}
}

///--------------------------------------------------------------
///						 再生再開
void MAudioG::ResumeWav(const std::string& filename) {
	std::lock_guard<std::mutex> lock(voiceMutex_);
	auto it = voiceMap_.find(filename);
	if (it != voiceMap_.end()) {
		it->second.sourceVoice->Start(0);
	}
}

///--------------------------------------------------------------
///						 音量を設定
void MAudioG::SetVolume(const std::string& filename, float volume) {
	float targetVolume = std::log(1.0f + volume) / std::log(2.0f); // 対数カーブを適用
	// 音量の最低ラインを決定
	targetVolume = std::max(targetVolume, 0.0f);

	std::lock_guard<std::mutex> lock(voiceMutex_);
	auto it = voiceMap_.find(filename);
	if (it != voiceMap_.end()) {
		if (targetVolume != it->second.oldVolume) {
			it->second.sourceVoice->SetVolume(targetVolume);
			it->second.oldVolume = targetVolume;
		}
	}
}

///--------------------------------------------------------------
///						 音量を設定(デシベル)
void MAudioG::SetVolumeDecibel(const std::string& filename, float dB) {
	float targetVolume = XAudio2DecibelsToAmplitudeRatio(dB);

	std::lock_guard<std::mutex> lock(voiceMutex_);
	auto it = voiceMap_.find(filename);
	if (it != voiceMap_.end()) {
		if (targetVolume != it->second.oldVolume) {
			it->second.sourceVoice->SetVolume(targetVolume);
			it->second.oldVolume = targetVolume;
		}
	}
}

///--------------------------------------------------------------
///						 再生速度を設定
void MAudioG::SetPlaybackSpeed(const std::string& filename, float speed) {
	std::lock_guard<std::mutex> lock(voiceMutex_);
	auto it = voiceMap_.find(filename);
	if (it != voiceMap_.end()) {
		if (speed != it->second.oldSpeed) {
			it->second.sourceVoice->SetFrequencyRatio(speed);
			it->second.oldSpeed = speed;
		}
	}
}

