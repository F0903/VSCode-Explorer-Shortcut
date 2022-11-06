#include <Windows.h>  

constexpr const wchar_t* DEFAULT_VSCODE_DIR = LR"(C:\Program Files\Microsoft VS Code\Code.exe)";

template<typename T>
struct Result {
	T value;
	int err_code;
};

const bool FileExists(const wchar_t* path) noexcept {
	auto attrs = GetFileAttributes(path);
	return (attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY));
}

Result<wchar_t* const> GetVSCodePath() noexcept {
	if (FileExists(DEFAULT_VSCODE_DIR)) {
		return { const_cast<wchar_t* const>(DEFAULT_VSCODE_DIR), 0 };
	}
	else {
		wchar_t env_buf[256];
		const auto env_len = GetEnvironmentVariable(L"VSCODE_EXE", env_buf, 256);
		if (env_len == 0) {
			return { nullptr, -2 };
		}
		return { env_buf, 0 };
	}
} 
 
int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, PWSTR cmdline, int cmd_show) {
	wchar_t dir_buf[256];
	const auto buf_len = GetCurrentDirectory(256, dir_buf);
	if (buf_len == 0) {
		return -1;
	}

	auto [vscode_dir, err_code] = GetVSCodePath();
	if (err_code != 0) {
		return err_code;
	}

	wchar_t cmd[512]{};
	lstrcpyW(cmd, vscode_dir);
	lstrcatW(cmd, L" ");
	lstrcatW(cmd, L"\"");
	lstrcatW(cmd, dir_buf);
	lstrcatW(cmd, L"\"");

	STARTUPINFO start_info = {};
	PROCESS_INFORMATION proc_info = {};
	start_info.cb = sizeof(STARTUPINFO);
	if (CreateProcess(NULL, cmd, NULL, NULL, FALSE, DETACHED_PROCESS, NULL, NULL, &start_info, &proc_info) == 0) {
		return -3;
	}
	return 0;
}