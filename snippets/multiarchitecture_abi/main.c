#include <unistd.h>

int main(int argc, char** argv)
{
    write(STDOUT_FILENO, "Hello World!", sizeof("Hello World!") - 1);
}
