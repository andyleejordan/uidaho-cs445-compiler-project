/* test chained logic statements and a switch */
int main(int argc, char *argv[])
{

	if (4 > 2 || (5 < 6 && 8 > 7) || !false) {
		int c = 42;
		switch (c) {
		case 1:
			1 + -3;
		case 2:
		case 3:
			-2 * 2;
			break;
		case 4:
			break;
		default:
			1 + 1;
			break;
		}
	}
	return 0;
}
