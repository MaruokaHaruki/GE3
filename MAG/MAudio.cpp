//
//  _____ _____ _____
// |     |     |   __| MAGSystem
// | | | |  |  |  |_ | Ver3.00
// |_|_|_|__|__|_____|
//
//

#define NOMINMAX
#include "MAudio.h"
#include <mmdeviceapi.h>
#include <Functiondiscoverykeys_devpkey.h>

///=====================================================///
/// シングルトンインスタンスを取得							///
///=====================================================///
MAudio* MAudio::GetInstance() {
	static MAudio instance;
	return &instance;
}

void MAudio::ListAudioDevices() {
	Microsoft::WRL::ComPtr<IMMDeviceEnumerator> deviceEnumerator;
	HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));
	if (FAILED(hr)) {
		std::cerr << "Failed to create device enumerator: " << std::hex << hr << std::endl;
		return;
	}

	Microsoft::WRL::ComPtr<IMMDeviceCollection> deviceCollection;
	hr = deviceEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &deviceCollection);
	if (FAILED(hr)) {
		std::cerr << "Failed to enumerate audio endpoints: " << std::hex << hr << std::endl;
		return;
	}

	UINT deviceCount;
	hr = deviceCollection->GetCount(&deviceCount);
	if (FAILED(hr)) {
		std::cerr << "Failed to get device count: " << std::hex << hr << std::endl;
		return;
	}

	audioDevices_.clear();

	for (UINT i = 0; i < deviceCount; ++i) {
		Microsoft::WRL::ComPtr<IMMDevice> device;
		hr = deviceCollection->Item(i, &device);
		if (FAILED(hr)) {
			std::cerr << "Failed to get device: " << std::hex << hr << std::endl;
			continue;
		}

		LPWSTR deviceId;
		hr = device->GetId(&deviceId);
		if (FAILED(hr)) {
			std::cerr << "Failed to get device ID: " << std::hex << hr << std::endl;
			continue;
		}

		Microsoft::WRL::ComPtr<IPropertyStore> propertyStore;
		hr = device->OpenPropertyStore(STGM_READ, &propertyStore);
		if (FAILED(hr)) {
			std::cerr << "Failed to open property store: " << std::hex << hr << std::endl;
			CoTaskMemFree(deviceId);
			continue;
		}

		PROPVARIANT friendlyName;
		PropVariantInit(&friendlyName);
		hr = propertyStore->GetValue(PKEY_Device_FriendlyName, &friendlyName);
		if (SUCCEEDED(hr)) {
			AudioDeviceInfo deviceInfo;
			deviceInfo.deviceId = deviceId;
			deviceInfo.displayName = friendlyName.pwszVal;

			audioDevices_.push_back(deviceInfo);

			std::wcout << L"Device " << i << L": " << friendlyName.pwszVal << L" (ID: " << deviceId << L")" << std::endl;
			PropVariantClear(&friendlyName);
		}

		CoTaskMemFree(deviceId);
	}
}
///=====================================================///
/// 初期化処理												///
///=====================================================///
void MAudio::Initialize(const std::string& directoryPath, const std::wstring& deviceId) {
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

///=====================================================///
/// 終了処理												///
///=====================================================///
void MAudio::Finalize() {
	// ロードされたサウンドデータをアンロード
	for (auto& soundData : soundDatas_) {
		if (!soundData.buffer.empty()) {
			Unload(&soundData);
		}
	}

	// 再生中のボイスを停止
	for (auto* voice : voices_) {
		if (voice->sourceVoice) {
			voice->sourceVoice->DestroyVoice();
			delete voice;
		}
	}

	// マスターボイスを破棄
	if (masterVoice_) {
		masterVoice_->DestroyVoice();
		masterVoice_ = nullptr;
	}

	// XAudio2のインスタンスをリセット
	xAudio2_.Reset();
}

///=====================================================///
/// WAVファイルの読み込み									///
///=====================================================///
uint32_t MAudio::LoadWave(const std::string& filename) {
	// ファイルをバイナリモードで開く
	std::ifstream file(directoryPath_ + filename, std::ios_base::binary);
	assert(file.is_open());

	// RIFFヘッダーを読み込む
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	assert(strncmp(riff.chunk.id, "RIFF", 4) == 0);
	assert(strncmp(riff.type, "WAVE", 4) == 0);

	// フォーマットチャンクを読み込む
	FormatChunk format = {};
	file.read((char*)&format, sizeof(ChunkHeader));
	assert(strncmp(format.chunk.id, "fmt ", 4) == 0);
	file.read((char*)&format.fmt, format.chunk.size);

	// データチャンクを読み込む
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	if (strncmp(data.id, "JUNK", 4) == 0) {
		file.seekg(data.size, std::ios_base::cur);
		file.read((char*)&data, sizeof(data));
	}
	assert(strncmp(data.id, "data", 4) == 0);

	// 音声データのバッファを読み込む
	std::vector<uint8_t> buffer(data.size);
	file.read(reinterpret_cast<char*>(buffer.data()), data.size);
	file.close();

	// サウンドデータを設定
	SoundData soundData = {};
	soundData.wfex = format.fmt;
	soundData.buffer = std::move(buffer);
	soundData.name = filename;

	// サウンドデータを配列に保存
	assert(indexSoundData_ < kMaxSoundData);
	soundDatas_[indexSoundData_] = std::move(soundData);

	return indexSoundData_++;
}

///=====================================================///
/// サウンドデータをアンロード
///=====================================================///
void MAudio::Unload(SoundData* soundData) {
	soundData->buffer.clear();
	soundData->name.clear();
}

///-------------------------------------------///
/// WAVファイルを再生
///-------------------------------------------///
uint32_t MAudio::PlayWave(uint32_t soundDataHandle, bool loopFlag, float volume, float maxPlaySpeed) {
	assert(soundDataHandle < kMaxSoundData);
	const SoundData& soundData = soundDatas_[soundDataHandle];

	// ソースボイスを作成
	IXAudio2SourceVoice* sourceVoice = nullptr;
	// 第1引数"現在鳴る音",第2引数"元データ",第3引数"フィルターの使用有無",第4引数"最大再生速度",で覚えておくべし
	HRESULT result = xAudio2_->CreateSourceVoice(&sourceVoice, &soundData.wfex, XAUDIO2_VOICE_USEFILTER, maxPlaySpeed, &voiceCallback_);
	assert(SUCCEEDED(result));

	// バッファを設定
	XAUDIO2_BUFFER buf = {};
	buf.pAudioData = soundData.buffer.data();
	buf.AudioBytes = (UINT32)soundData.buffer.size();
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.LoopCount = loopFlag ? XAUDIO2_LOOP_INFINITE : 0;

	// ソースボイスにバッファを送信
	result = sourceVoice->SubmitSourceBuffer(&buf);
	assert(SUCCEEDED(result));

	// 再生開始
	result = sourceVoice->Start(0);
	assert(SUCCEEDED(result));

	// ボイスを作成し、セットに追加
	Voice* voice = new Voice();
	voice->handle = indexVoice_;
	voice->sourceVoice = sourceVoice;
	voice->sourceVoice->SetVolume(volume);

	std::lock_guard<std::mutex> lock(voiceMutex_);
	voices_.insert(voice);

	return indexVoice_++;
}

///-------------------------------------------///
/// WAVファイルを再生(詳細設定可)
///-------------------------------------------///
// NOTE:サンプリングレートとかを取得しないと問題が起きるぞ。サウンドの人と喧嘩しな！
uint32_t MAudio::PlayWaveDetail(uint32_t soundDataHandle, bool loopFlag , float volume, float maxPlaySpeed,
		float startTime , float endTime, float loopStartTime, float loopEndTime) { 

	assert(soundDataHandle < kMaxSoundData);
	const SoundData& soundData = soundDatas_[soundDataHandle];

	// ソースボイスを作成
	IXAudio2SourceVoice* sourceVoice = nullptr;
	// 第1引数"現在鳴る音",第2引数"元データ",第3引数"フィルターの使用有無",第4引数"最大再生速度",で覚えておくべし
	HRESULT result = xAudio2_->CreateSourceVoice(&sourceVoice, &soundData.wfex, XAUDIO2_VOICE_USEFILTER, maxPlaySpeed, &voiceCallback_);
	assert(SUCCEEDED(result));

	// バッファを設定
	XAUDIO2_BUFFER buf = {};
	buf.pAudioData = soundData.buffer.data();
	buf.AudioBytes = (UINT32)soundData.buffer.size();

	//再生開始場所
	buf.PlayBegin = static_cast<UINT32>(startTime * waveSamplingRate);
	buf.PlayLength = static_cast<UINT32>(endTime * waveSamplingRate);
	buf.LoopBegin = static_cast<UINT32>(loopStartTime * waveSamplingRate);
	buf.LoopLength = static_cast<UINT32>(loopEndTime * waveSamplingRate);

	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.LoopCount = loopFlag ? XAUDIO2_LOOP_INFINITE : 0;

	// ソースボイスにバッファを送信
	result = sourceVoice->SubmitSourceBuffer(&buf);
	assert(SUCCEEDED(result));

	// 再生開始
	result = sourceVoice->Start(0);
	assert(SUCCEEDED(result));

	// ボイスを作成し、セットに追加
	Voice* voice = new Voice();
	voice->handle = indexVoice_;
	voice->sourceVoice = sourceVoice;
	voice->sourceVoice->SetVolume(volume);

	std::lock_guard<std::mutex> lock(voiceMutex_);
	voices_.insert(voice);

	return indexVoice_++;
}

///-------------------------------------------///
/// WAVファイルをセットする
///-------------------------------------------///
uint32_t MAudio::SetWave(uint32_t soundDataHandle, bool loopFlag, float volume, float maxPlaySpeed) {
	assert(soundDataHandle < kMaxSoundData);
	const SoundData& soundData = soundDatas_[soundDataHandle];

	// ソースボイスを作成
	IXAudio2SourceVoice* sourceVoice = nullptr;
	HRESULT result = xAudio2_->CreateSourceVoice(&sourceVoice, &soundData.wfex, 0, maxPlaySpeed, &voiceCallback_);
	assert(SUCCEEDED(result));

	// バッファを設定
	XAUDIO2_BUFFER buf = {};
	buf.pAudioData = soundData.buffer.data();
	buf.AudioBytes = (UINT32)soundData.buffer.size();
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.LoopCount = loopFlag ? XAUDIO2_LOOP_INFINITE : 0;

	// ソースボイスにバッファを送信
	result = sourceVoice->SubmitSourceBuffer(&buf);
	assert(SUCCEEDED(result));

	// ボイスを作成し、セットに追加
	Voice* voice = new Voice();
	voice->handle = indexVoice_;
	voice->sourceVoice = sourceVoice;
	voice->sourceVoice->SetVolume(volume);

	std::lock_guard<std::mutex> lock(voiceMutex_);
	voices_.insert(voice);

	return indexVoice_++;
}

///-------------------------------------------///
/// 再生を停止
///-------------------------------------------///
void MAudio::StopWave(uint32_t voiceHandle) {
	std::lock_guard<std::mutex> lock(voiceMutex_);
	for (auto* voice : voices_) {
		if (voice->handle == voiceHandle) {
			voice->sourceVoice->Stop();
			voice->sourceVoice->DestroyVoice();
			voices_.erase(voice);
			delete voice;
			break;
		}
	}
}

///-------------------------------------------///
/// 再生中かどうかを確認
///-------------------------------------------///
bool MAudio::IsPlaying(uint32_t voiceHandle) {
	std::lock_guard<std::mutex> lock(voiceMutex_);
	for (auto* voice : voices_) {
		if (voice->handle == voiceHandle) {
			XAUDIO2_VOICE_STATE state;
			voice->sourceVoice->GetState(&state);
			return state.BuffersQueued > 0;
		}
	}
	return false;
}

///-------------------------------------------///
/// 再生を一時停止
///-------------------------------------------///
void MAudio::PauseWave(uint32_t voiceHandle) {
	std::lock_guard<std::mutex> lock(voiceMutex_);
	for (auto* voice : voices_) {
		if (voice->handle == voiceHandle) {
			voice->sourceVoice->Stop();
			break;
		}
	}
}

///-------------------------------------------///
/// 再生を再開
///-------------------------------------------///
void MAudio::ResumeWave(uint32_t voiceHandle) {
	std::lock_guard<std::mutex> lock(voiceMutex_);
	for (auto* voice : voices_) {
		if (voice->handle == voiceHandle) {
			voice->sourceVoice->Start(0);
			break;
		}
	}
}

///-------------------------------------------///
/// 音量を設定
///-------------------------------------------///
 float Volume[] = {1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
void MAudio::SetVolume(uint32_t voiceHandle, float volume) {
	float targetVolume = std::log(1.0f + volume) / std::log(2.0f); // 対数カーブを適用
	// 音量の最低ラインを決定
	// NOTE:-は位相が逆転して音量が戻っていくぞ
	targetVolume = std::max(targetVolume, 0.0f);

	XAUDIO2_VOICE_DETAILS details;

	// ソースボイスの格納場所を探す
	std::lock_guard<std::mutex> lock(voiceMutex_);
	for (auto* voice : voices_) {
		if (voice->handle == voiceHandle) {
			if (targetVolume != voice->oldVolume) {

				voice->sourceVoice->SetVolume(targetVolume);
				voice->oldVolume = targetVolume;

				 //実験中(現在取りやめ)
				 voice->sourceVoice->GetVoiceDetails(&details);//ソ-スボイスの情報取得
				 int inChannel = details.InputChannels;
				 masterVoice_->GetVoiceDetails(&details);
				 int outChannel = details.InputChannels;

				 voice->sourceVoice->SetOutputMatrix(NULL, inChannel, outChannel, Volume);
			}
			break;
		}
	}
}

///-------------------------------------------///
/// 音量を設定(デシベル)
///-------------------------------------------///
// NOTE:これでサウンドデザイナーを黙らせろ!
void MAudio::SetVolumeDecibel(uint32_t voiceHandle, float dB) {
	float targetVolume = XAudio2DecibelsToAmplitudeRatio(dB);
	// ソースボイスの格納場所を探す
	std::lock_guard<std::mutex> lock(voiceMutex_);
	for (auto* voice : voices_) {
		if (voice->handle == voiceHandle) {
			if (targetVolume != voice->oldVolume) {
				voice->sourceVoice->SetVolume(targetVolume);
				voice->oldVolume = targetVolume;
			}
			break;
		}
	}
}

///-------------------------------------------///
/// 再生速度を設定
///-------------------------------------------///
// NOTE:ピッチが上がるぞ
void MAudio::SetPlaybackSpeed(uint32_t voiceHandle, float speed) {
	std::lock_guard<std::mutex> lock(voiceMutex_);
	for (auto* voice : voices_) {
		if (voice->handle == voiceHandle) {
			voice->sourceVoice->SetFrequencyRatio(speed);
			voice->oldSpeed = speed;
			break;
		}
	}
}
