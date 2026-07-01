namespace engine_core::test {
	__declspec(dllimport) void run_tests();
}

int main() {
	engine_core::test::run_tests();
	return 0;
}
