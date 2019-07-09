#include "OS/file.hpp"
#include <Windows.h>

xstd::std_expected<std::string> file::read_whole_text(const std::filesystem::path& path) noexcept {
	auto handle = CreateFile(
		path.native().c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		nullptr
	);
	if (handle == INVALID_HANDLE_VALUE) {
		return Error::Win_Create_File;
	}
	defer{ CloseHandle(handle); };

	LARGE_INTEGER large_int;
	GetFileSizeEx(handle, &large_int);
	if (large_int.QuadPart == 0) {
		return Error::Win_File_Size;
	}

	std::string buffer;
	buffer.reserve((std::size_t)large_int.QuadPart);

	DWORD read;
	if (!ReadFile(handle, buffer.data(), (DWORD)large_int.QuadPart, &read, nullptr)) {
		return Error::Win_File_Read;
	}

	return buffer;
}
