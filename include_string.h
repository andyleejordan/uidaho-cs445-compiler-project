class string {
public:
	string(char *);
	char *c_str();
private:
	char *data;
};

string::string(char *s)
{
	data = s;
}
