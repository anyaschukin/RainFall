#include <stdlib.h>
#include <cstring>

class N {
	public:
		char annotation[100];
		int nb;

		N(int n) {
			nb = n;
		}
		void setAnnotation(char *str) {
			memcpy(annotation, str, strlen(str));
		}
		virtual int operator+(N &n) {
			return (nb + n.nb);
		}
		virtual int operator-(N &n) {
			return (nb - n.nb);
		}
};

int main(int argc, char **argv) {
	if (argc <= 1)
		exit(1);

	N *n1 = new N(5);
	N *n2 = new N(6);

	N *n1_ptr = n1;
	N *n2_ptr = n2;

	n1_ptr->setAnnotation(argv[1]);
	return (*n2_ptr + *n1_ptr);
}