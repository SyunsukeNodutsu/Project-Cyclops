//-----------------------------------------------------------------------------
// File: Timer.h
//
// ���Ԍv��
//-----------------------------------------------------------------------------
#pragma once

//���Ԍv���^�C�}�[
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
