//  _____ _____ _____
// |     |     |   __| MagAudio
// | | | |  |  |  |_ | Ver4.10
// |_|_|_|__|__|_____| 2024/09/23
//

#pragma once
#define XAUDIO2_HELPER_FUNCTIONS
#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")
#include <array>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <mutex>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include <wrl.h>

///=============================================================================
///						サウンドセット
struct SoundSet final {
	//std::string fileName;
	int dataHandle;
	uint32_t voiceHandle;
	float volume = 0.0f;
};

///=============================================================================
///						クラス
class MagAudio {
public:
	///--------------------------------------------------------------
	///						 サウンドデータの最大数
	static const int kMaxSoundData = 256;

	///--------------------------------------------------------------
	///						 デバイス情報を格納する構造体の定義
	struct AudioDeviceInfo {
		std::wstring deviceId;
		std::wstring displayName;
	};

	///--------------------------------------------------------------
	///						 音声データを表す構造体
	struct SoundData {
		WAVEFORMATEX wfex;           // 波形フォーマット
		std::vector<uint8_t> buffer; // 音声データのバッファ
		std::string name;            // 音声ファイルの名前
	};

	///--------------------------------------------------------------
	///						 再生データを表す構造体
	struct Voice {
		uint32_t handle = 0u;                       // ハンドル（識別子）
		IXAudio2SourceVoice* sourceVoice = nullptr; // XAudio2のソースボイス
		float oldVolume;                            // 最後に設定したボリューム
		float oldSpeed;                             // 最後に設定したボリューム
	};

	///--------------------------------------------------------------
	///						 チャンクヘッダーを表す構造体
	struct ChunkHeader {
		char id[4];    // チャンクのID（"RIFF", "fmt ", "data"など）
		uint32_t size; // チャンクのサイズ
	};

	///--------------------------------------------------------------
	///						 RIFFヘッダーを表す構造体
	struct RiffHeader {
		ChunkHeader chunk; // チャンクヘッダー
		char type[4];      // ファイルタイプ（"WAVE"）
	};

	///--------------------------------------------------------------
	///						 フォーマットチャンクを表す構造体
	struct FormatChunk {
		ChunkHeader chunk; // チャンクヘッダー
		WAVEFORMATEX fmt;  // 波形フォーマット
	};

	///--------------------------------------------------------------
	///						 オーディオコールバックを表すクラス
	class XAudio2VoiceCallback : public IXAudio2VoiceCallback {
	public:
		STDMETHOD_(void, OnVoiceProcessingPassStart)( UINT32 BytesRequired ) override { UNREFERENCED_PARAMETER(BytesRequired); }
		STDMETHOD_(void, OnVoiceProcessingPassEnd)( ) override {}
		STDMETHOD_(void, OnStreamEnd)( ) override {}
		STDMETHOD_(void, OnBufferStart)( void* pBufferContext ) override { UNREFERENCED_PARAMETER(pBufferContext); }
		STDMETHOD_(void, OnBufferEnd)( void* pBufferContext ) override { UNREFERENCED_PARAMETER(pBufferContext); }
		STDMETHOD_(void, OnLoopEnd)( void* pBufferContext ) override { UNREFERENCED_PARAMETER(pBufferContext); }
		STDMETHOD_(void, OnVoiceError)( void* pBufferContext, HRESULT Error ) override {
			UNREFERENCED_PARAMETER(pBufferContext);
			std::cerr << "Voice error: " << std::hex << Error << std::endl;
		}
	};

	MagAudio() = default;
	~MagAudio() { Finalize(); }
	MagAudio(const MagAudio&) = delete;
	const MagAudio& operator=(const MagAudio&) = delete;

	/**----------------------------------------------------------------------------
	 * \brief  GetInstance		シングルトンインスタンス
	 * \return 
	 */
	static MagAudio* GetInstance();

	/**----------------------------------------------------------------------------
	 * \brief  GetAudioDevices	接続デバイスの検知
	 */
	void GetAudioDevices();

	/**----------------------------------------------------------------------------
	 * \brief  Initialize		初期化
	 * \param  directoryPath	ディレクトリパス
	 * \param  deviceId			デバイスID
	 */
	void Initialize(const std::string& directoryPath = "Resources/", const std::wstring& deviceId = L"");

	/**----------------------------------------------------------------------------
	 * \brief  Finalize		終了処理
	 */
	void Finalize();

	/**----------------------------------------------------------------------------
	 * \brief  LoadSound	サウンドのロード
	 * \param  filename		ファイル名
	 * \return 
	 */
	uint32_t LoadSound(const std::string& filename);

	/**----------------------------------------------------------------------------
	 * \brief  Unload		サウンドのアンロード
	 * \param  soundData	サウンドデータ
	 */
	void Unload(SoundData* soundData);

	/**----------------------------------------------------------------------------
	 * \brief  PlayingSound		WAVファイルを再生
	 * \param  soundDataHandl	格納場所の番号
	 * \param  loopFlag			ループするかどうか
	 * \param  volume			ボリューム
	 * \param  maxPlaySpeed		最大再生速度
	 * \return					ボイスハンドル
	 */
	uint32_t PlayingSound(uint32_t soundDataHandle, 
		bool loopFlag = false,
		float volume = 1.0f,
		float maxPlaySpeed = 2.0f);

	/**----------------------------------------------------------------------------
	 * \brief  PlaySoundDetail	WAVファイルを再生(詳細設定可)
	 * \param  soundDataHandle	格納場所の番号
	 * \param  loopFlag			ループするかどうか
	 * \param  volume			ボリューム
	 * \param  maxPlaySpeed		最大再生速度
	 * \param  startTime		再生開始場所
	 * \param  endTime			再生終了場所
	 * \param  loopStartTime	ループ開始場所
	 * \param  loopEndTime		ループ終了場所
	 * \return 
	 */
	uint32_t PlaySoundDetail(
		uint32_t soundDataHandle,
		bool loopFlag = false, 
		float volume = 1.0f, 
		float maxPlaySpeed = 2.0f, 
		float startTime = 0.0f, 
		float endTime = 0.0f, 
		float loopStartTime = 0.0f,
		float loopEndTime = 0.0f);

	/**----------------------------------------------------------------------------
	 * \brief  SetSound			WAVファイルをセットする
	 * \param  soundDataHandle	格納場所の番号
	 * \param  loopFlag			ループするかどうか
	 * \param  volume			ボリューム
	 * \param  maxPlaySpeed		最大再生速度
	 * \return ボイスハンドル
	 */
	uint32_t SetSound(uint32_t soundDataHandle, 
		bool loopFlag = false, 
		float volume = 1.0f, 
		float maxPlaySpeed = 2.0f);

	/**----------------------------------------------------------------------------
	 * \brief  StopSound 再生を停止
	 * \param  voiceHandle ボイスハンドル
	 */
	void StopSound(uint32_t voiceHandle);

	/**----------------------------------------------------------------------------
	 * \brief  IsPlaying 再生中かどうかを確認
	 * \param  voiceHandle ボイスハンドル
	 * \return 
	 */
	bool IsPlaying(uint32_t voiceHandle);

	/**----------------------------------------------------------------------------
	 * \brief  PauseSound 再生一時停止
	 * \param  voiceHandle ボイスハンドル
	 */
	void PauseSound(uint32_t voiceHandle);

	/**----------------------------------------------------------------------------
	 * \brief  ResumeSound 再生再開
	 * \param  voiceHandle　ボイスハンドル
	 */
	void ResumeSound(uint32_t voiceHandle);
	
	/**----------------------------------------------------------------------------
	 * \brief  SetVolume	音量を設定
	 * \brief  NOTE: 0が無音,1が元の音源そのまま,0.3fくらいから判断
	 * \param  voiceHandle	格納場所の番号
	 * \param  volume		音量
	 */
	void SetVolume(uint32_t voiceHandle, float volume);

	/**----------------------------------------------------------------------------
	 * \brief  SetVolumeDecibel 音量を設定(デシベル)
	 * \param  voiceHandle ボイスハンドル
	 * \param  dB デシベル
	 */
	void SetVolumeDecibel(uint32_t voiceHandle, float dB);

	/**----------------------------------------------------------------------------
	 * \brief  SetPlaybackSpeed 再生速度を設定
	 * \param  voiceHandle		ボイスハンドル
	 * \param  speed			再生速度
	 */
	void SetPlaybackSpeed(uint32_t voiceHandle, float speed);

private:

	std::vector<AudioDeviceInfo> audioDevices_; // 出力オーディオ

	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;      // XAudio2インターフェース
	IXAudio2MasteringVoice* masterVoice_ = nullptr; // マスターボイス

	std::array<SoundData, kMaxSoundData> soundDatas_; // サウンドデータの配列
	std::set<Voice*> voices_;                         // 再生中のボイスのセット
	std::string directoryPath_;                       // 音声ファイルのディレクトリパス

	uint32_t indexSoundData_ = 0u; // サウンドデータのインデックス
	uint32_t indexVoice_ = 0u;     // ボイスのインデックス

	XAudio2VoiceCallback voiceCallback_; // オーディオコールバック

	std::mutex voiceMutex_; // ボイス操作のためのミューテックス

	float waveSamplingRate; // サンプリングレート
};
