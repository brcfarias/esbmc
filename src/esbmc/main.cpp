#include <esbmc/esbmc_parseoptions.h>

int main(int argc, const char **argv)
{
  esbmc_parseoptionst parseoptions(argc, argv);
  return parseoptions.main();
}
