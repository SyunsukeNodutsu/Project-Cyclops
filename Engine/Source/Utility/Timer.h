//-----------------------------------------------------------------------------
// File: Timer.h
//
// �e��^�C�}�[
// TODO: FpsTimer��Input�Ɠ��l�E�B���h�E�̃��[�v�Ɉˑ�
//-----------------------------------------------------------------------------
#pragma once

//�ėp���Ԍv���^�C�}�[
struct CommonTimer
{
	//@brief �R���X�g���N�^ ���R�[�h�̊J�n
	CommonTimer() { Record(); }

	//@brief ��ƂȂ�^�C���X�^���v�̋L�^
	inline void Record() {
		m_timeStamp = std::chrono::high_resolution_clock::now();
	}

	//@brief �Ō��Record()���Ă΂�Ă���̌o�ߎ���(�b)��Ԃ�
	//@return �o�ߎ���(�b)
	inline double GetElapsedSeconds() {
		auto now = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(now - m_timeStamp);
		return time_span.count();
	}

	//@brief �Ō��Record()���Ă΂�Ă���̌o�ߎ���(�~���b)��Ԃ�
	//@return �o�ߎ���(�~���b)
	inline double GetElapsedMilliseconds() {
		return GetElapsedSeconds() * 1000.0;
	}

private:

	std::chrono::high_resolution_clock::time_point m_timeStamp;

};

//FPS�v���p�^�C�}�[
//TODO: ���Ԃ̃X�P�[�����O�͂����ŊǗ�����ׂ��ł͂Ȃ�����
//TODO: static�ł���
class FpsTimer
{
public:

	FpsTimer();
	~FpsTimer() = default;

	void Tick();
	void ResetDeltaTime();

	double GetTimeScale() const { return m_scaling; }
	double GetDeltaTime(bool modeRaw = false) const { return TicksToSeconds(m_deltaTicks) * (modeRaw ? 1 : m_scaling); }
	double GetTotalTime() const { return TicksToSeconds(m_totalTicks); }

	std::uint32_t GetFPS() const { return m_fps; }
	std::uint32_t GetTotalFrameCount() const { return m_totalFrameCount; }

	void SetTimeScale(double scale) { m_scaling = scale; }

	static constexpr std::uint64_t TicksPerSecond = 10'000'000;
	static double TicksToSeconds(const std::uint64_t ticks) { return static_cast<double>(ticks) / TicksPerSecond; }
	static std::uint64_t SecondsToTicks(const double seconds) { return static_cast<std::uint64_t>(seconds * TicksPerSecond); }

public:

	static std::uint64_t DeltaTime;

private:

	LARGE_INTEGER	m_frequency;		//���𑜓x�^�C�}�̎��g��
	LARGE_INTEGER	m_lastTime;			//�O��v������

	std::uint64_t	m_maxDelta;			//���e����ő�̃f���^�^�C��
	std::uint64_t	m_deltaTicks;		//�O�t���[������̃f���^�e�B�b�N
	std::uint64_t	m_totalTicks;		//���̃^�C�}�̑��o�߃e�B�b�N

	std::uint32_t	m_totalFrameCount;	//���t���[����
	std::uint32_t	m_frameThisCount;	//���݂�1�b�̃t���[�����𐔂���

	std::uint32_t	m_fps;				//�O��1�b�̃t���[�����[�g
	std::uint64_t	m_secondCounter;	//1�b���v��

	double			m_scaling;			//���Ԃ̌o�߂��X�P�[�����O

};

