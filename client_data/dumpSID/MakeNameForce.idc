static MakeNameForce (address, name) {
	auto x = 1;
	auto newName = name;
	while (MakeNameEx (address, newName, 1) == 0) {
		newName = sprintf("%s_%d", name, x);
		x = x + 1;
	}
}