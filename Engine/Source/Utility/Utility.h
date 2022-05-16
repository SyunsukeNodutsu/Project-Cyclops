//-----------------------------------------------------------------------------
// File: Utility.h
//
// �֗��@�\
//-----------------------------------------------------------------------------
#pragma once

//�֗��}�N��
#define ToString(Val) std::to_string(Val)//std::to_string���G�C���A�X

//�֗��@�\�N���X
class Utility
{
public:

    //@brief ���S�p�X����t�@�C����(�g���q�܂�)��Ԃ�
    static const std::string& GetFilenameFromFullpath(const std::string& fullpath);

    //@brief ���S�p�X����t�@�C����(�g���q�܂�)��Ԃ�
    static const std::wstring& GetFilenameFromFullpath(const std::wstring& fullpath);

};

//�f�o�b�O�N���X
class Debug
{
public:

    //@brief ���O�𓊂���
    //@param location ���O�ɏo�͂��郍�P�[�V������� ���̒l�͐ݒ肵�Ȃ�
    static void Log(std::string log, std::source_location location = std::source_location::current());

    //@brief ���O�𓊂���
    //@param location ���O�ɏo�͂��郍�P�[�V������� ���̒l�͐ݒ肵�Ȃ�
    static void Log(std::wstring log, std::source_location location = std::source_location::current());

};
