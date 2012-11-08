#include "clockreader.hh"

int main(int argc, char** argv)
{

  if (argc != 3)
  {
    std::cerr << "Wrong number of argument:" << std::endl
	      << "expected: ./clockreader [mode] path/to/img" << std::endl;

    return 1;
  }


  std::string filename(argv[2]);
  std::string mode(argv[1]);


  if (mode == "detect")
  {
    std::vector<Mask> masks = clock_detect(filename);

    std::vector<Mask>::iterator end = masks.end();
    std::vector<Mask>::iterator it = masks.begin();

    for (; it != end; ++it)
      std::cout << (*it) << std::endl;

    return 0;
  }
  else if (mode == "reader")
  {
    std::vector<Mask> masks = read_masks();
    print_masks(masks);

    hourvector hours = readclock(masks, filename);

    hourvector::iterator end = hours.end();
    hourvector::iterator it = hours.begin();

    for (; it != end; ++it)
      std::cout << *it << std::endl;
  }
  else if (mode == "full")
  {
    std::vector<Mask> masks = clock_detect(filename);

    hourvector hours = readclock(masks, filename);

    hourvector::iterator end = hours.end();
    hourvector::iterator it = hours.begin();

    for (; it != end; ++it)
      std::cout << *it << std::endl;
  }
  else
  {
    std::cerr << "Mode not found!" << std::endl;
    return 2;
  }

  return 0;
}
