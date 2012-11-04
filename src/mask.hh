#ifndef MASK_HH_
# define MASK_HH_

# include <vector>
# include <iostream>

class Mask
{
  public:
};

std::ostream& operator<< (std::ostream& ostr, Mask&) { return ostr; }

std::vector<Mask> read_masks()
{
  return std::vector<Mask> ();
}


#endif // !MASK_HH_
