// TODO: #ifndef ������ �ٲٱ� (�ٸ�����)
#pragma once

// TODO: ������� ���� �Ծ࿡ �°� �ٲٱ� (�ٸ�����)
#include <string>

namespace remoteFileExplorer
{
namespace common
{

class FileExplorerInterface
{
public:
	virtual ~FileExplorerInterface() = default;

	// �׽�Ʈ�뵵.
	virtual std::string Echo(const char* str) = 0;
};

} // namespace common
} // namespace remoteFileExplorer