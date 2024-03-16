#pragma once

void start_driver()
{
	driver().handle_driver();

	if (!driver().is_loaded())
	{
		cout << xorstr_("\n driver initialize...") << endl;
		mmap_driver();
	}

	driver().handle_driver();
	driver().is_loaded() ? cout << xorstr_("\n driver initialized!") << endl : cout << xorstr_("\n driver initialize error =<") << endl;
}

