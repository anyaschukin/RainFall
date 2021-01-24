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

int main(int ac, char **av) {
	if (ac <= 1)
		exit(1);

	N *n1 = new N(5);
	N *n2 = new N(6);

	N *n_ptr1 = n1;
	N *n_ptr2 = n2;

	n_ptr1->setAnnotation(av[1]);
	return (*n_ptr2 + *n_ptr1);
}