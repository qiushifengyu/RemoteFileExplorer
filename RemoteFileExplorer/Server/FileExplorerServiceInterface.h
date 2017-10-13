#pragma once

#include <memory>

#include "Common/FileExplorerInterface.h"

namespace remoteFileExplorer
{
namespace server
{
///////////////////////////////////////////////////////////////////////////////
// ���� Ž���� ���񽺿� ���� �������̽�.
// ������ ���ÿ��� ���� Ž���⿡ ���õ� ��ɵ��� �����ϴ� �Ϳ� ���� �������̽�.
class FileExplorerServiceInterface : public common::FileExplorerInterface
{
public:
	// ���ο� Ŭ���̾�Ʈ�� �����ϸ�, ���� Ž���� ���� ��ü�� �����Ǿ�
	//   �� Ŭ���̾�Ʈ�� ����ǰ� �ȴ�.
	// �׸��� ����� ���� ��ü�� �� Ŭ���̾�Ʈ�� ���� ������ ��ü�� ��ȭ�� ���̴�.
	//   (���� ���� ������ �ʿ伺�� ������, ���ĸ� ����� �����̴�.)
	virtual std::unique_ptr<FileExplorerServiceInterface> Clone() const = 0;
};

} // namespace server
} // namespace remoteFileExplorer