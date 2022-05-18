#include "Timer.h"

//-----------------------------------------------------------------------------
// �R���X�g���N�^
//-----------------------------------------------------------------------------
FpsTimer::FpsTimer()
	: m_frequency()
	, m_lastTime()
	, m_maxDelta(0)
	, m_deltaTicks(0)
	, m_totalTicks(0)
	, m_totalFrameCount(0)
	, m_frameThisCount(0)
	, m_fps(0)
	, m_secondCounter(0)
	, m_scaling(1.0f)
{
	//���g���ۑ�
	QueryPerformanceFrequency(&m_frequency);
	QueryPerformanceCounter(&m_lastTime);

	m_maxDelta = m_frequency.QuadPart / 10;
}

//-----------------------------------------------------------------------------
// �O�t���[������̃f���^�^�C�����v�Z
//-----------------------------------------------------------------------------
void FpsTimer::Tick()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);

	//�O���Tick����̌o�ߎ���
	std::uint64_t deltaTime = currentTime.QuadPart - m_lastTime.QuadPart;
	m_lastTime = currentTime;
	m_secondCounter += deltaTime;

	//���炩�̗��R�őO�񂩂玞�Ԃ����������Ă���Ƃ���m_maxDelta��delta_time�ɂ���
	//���̂܂܎g���Ǝ��Ԃ��傫�����ăQ�[���̏������j�]����̂�h��
	if (deltaTime > m_maxDelta) {
		deltaTime = m_maxDelta;
		Debug::Log("���e�͈͂𒴂���.");
	}

	//���ԂƂ��Ĉ�����悤�Ɍv�Z
	deltaTime = deltaTime * TicksPerSecond / m_frequency.QuadPart;

	//���ꂼ��X�V
	m_deltaTicks = deltaTime;
	m_totalTicks += deltaTime;
	++m_totalFrameCount;
	++m_frameThisCount;

	//1�b�o�߂�����Ăяo���ꂽTick()�̐���fps�Ƃ��� ��FPS��1�b�X�V
	if (static_cast<std::uint64_t>(m_frequency.QuadPart) <= m_secondCounter)
	{
		m_fps = m_frameThisCount;
		m_frameThisCount = 0;

		//0�ɂ���ƒ[���̎��Ԃ������ă^�C�}�����X�ɋ���
		//��]�Œ[���̎��Ԃ�ۑ����Ă���
		m_secondCounter %= m_frequency.QuadPart;
	}
}

//-----------------------------------------------------------------------------
// �o�ߎ��ԃ��Z�b�g
//-----------------------------------------------------------------------------
void FpsTimer::ResetDeltaTime()
{
	QueryPerformanceCounter(&m_lastTime);
	m_frameThisCount = 0;
	m_fps = 0;
	m_secondCounter = 0;
}
