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

std::ostream& operator<< (std::ostream& ostr, const Mask& m) {
  ostr << m.x_ << " " << m.y_ << " " << m.major_rad_ << " " << m.minor_rad_
       << " " << m.rotation_;

  return ostr;
}

void print_masks(const std::vector<Mask>& parMasks)
{
  std::vector<Mask>::const_iterator it = parMasks.begin();
  std::vector<Mask>::const_iterator end = parMasks.end();

  for (; it != end; ++it)
  {
    std::cout << *it << std::endl;
  }
}

std::vector<Mask> read_masks()
{
  std::vector<Mask> masks;

  while (!std::cin.eof())
  {
    int x = -1;
    int y = -1;
    int a = -1;
    int b = -1;
    int r = -1;
    std::cin >> x >> y >> a >> b >> r;

    if (x == -1 || y == -1 || a == -1 || b == -1)
      break;

    masks.push_back(Mask(x, y, a, b, r));
  }

  return masks;
}

#endif // !MASK_HH_
