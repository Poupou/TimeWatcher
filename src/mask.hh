#ifndef MASK_HH_
# define MASK_HH_

# include <vector>
# include <iostream>

class Mask
{
  public:
    Mask(int x, int y, int major_rad, int minor_rad, int rotation)
      : x_(x), y_(y), major_rad_(major_rad), minor_rad_(minor_rad), rotation_(rotation)
    {}

  public:
    int x_;
    int y_;
    int major_rad_;
    int minor_rad_;
    int rotation_;

  friend std::ostream& operator<< (std::ostream& ostr, const Mask&);
};

std::ostream& operator<< (std::ostream& ostr, const Mask& m);
void print_masks(const std::vector<Mask>& parMasks);
std::vector<Mask> read_masks();

#endif // !MASK_HH_
