//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// strcmp.cpp
//
// Code generation for function 'strcmp'
//

// Include files
#include "strcmp.h"
#include "reflectivityCalculation_data.h"
#include "rt_nonfinite.h"

// Function Definitions
namespace RAT
{
  namespace coder
  {
    namespace internal
    {
      boolean_T b_strcmp(const char_T a_data[], const int32_T a_size[2])
      {
        static const char_T b_cv[13]{ 'n', 'o', 'n', ' ', 'p', 'o', 'l', 'a',
          'r', 'i', 's', 'e', 'd' };

        boolean_T b_bool;
        b_bool = false;
        if (a_size[1] == 13) {
          int32_T kstr;
          kstr = 0;
          int32_T exitg1;
          do {
            exitg1 = 0;
            if (kstr < 13) {
              if (a_data[kstr] != b_cv[kstr]) {
                exitg1 = 1;
              } else {
                kstr++;
              }
            } else {
              b_bool = true;
              exitg1 = 1;
            }
          } while (exitg1 == 0);
        }

        return b_bool;
      }

      boolean_T c_strcmp(const char_T a_data[], const int32_T a_size[2])
      {
        static const char_T b_cv[15]{ 's', 't', 'a', 'n', 'd', 'a', 'r', 'd',
          ' ', 'l', 'a', 'y', 'e', 'r', 's' };

        boolean_T b_bool;
        b_bool = false;
        if (a_size[1] == 15) {
          int32_T kstr;
          kstr = 0;
          int32_T exitg1;
          do {
            exitg1 = 0;
            if (kstr < 15) {
              if (a_data[kstr] != b_cv[kstr]) {
                exitg1 = 1;
              } else {
                kstr++;
              }
            } else {
              b_bool = true;
              exitg1 = 1;
            }
          } while (exitg1 == 0);
        }

        return b_bool;
      }

      boolean_T d_strcmp(const char_T a_data[], const int32_T a_size[2])
      {
        static const char_T b_cv[13]{ 'c', 'u', 's', 't', 'o', 'm', ' ', 'l',
          'a', 'y', 'e', 'r', 's' };

        boolean_T b_bool;
        b_bool = false;
        if (a_size[1] == 13) {
          int32_T kstr;
          kstr = 0;
          int32_T exitg1;
          do {
            exitg1 = 0;
            if (kstr < 13) {
              if (a_data[kstr] != b_cv[kstr]) {
                exitg1 = 1;
              } else {
                kstr++;
              }
            } else {
              b_bool = true;
              exitg1 = 1;
            }
          } while (exitg1 == 0);
        }

        return b_bool;
      }

      boolean_T e_strcmp(const char_T a_data[], const int32_T a_size[2])
      {
        static const char_T b_cv[9]{ 'c', 'u', 's', 't', 'o', 'm', ' ', 'x', 'y'
        };

        boolean_T b_bool;
        b_bool = false;
        if (a_size[1] == 9) {
          int32_T kstr;
          kstr = 0;
          int32_T exitg1;
          do {
            exitg1 = 0;
            if (kstr < 9) {
              if (a_data[kstr] != b_cv[kstr]) {
                exitg1 = 1;
              } else {
                kstr++;
              }
            } else {
              b_bool = true;
              exitg1 = 1;
            }
          } while (exitg1 == 0);
        }

        return b_bool;
      }

      boolean_T f_strcmp(const char_T a_data[], const int32_T a_size[2])
      {
        static const char_T b_cv[6]{ 's', 'i', 'n', 'g', 'l', 'e' };

        boolean_T b_bool;
        b_bool = false;
        if (a_size[1] == 6) {
          int32_T kstr;
          kstr = 0;
          int32_T exitg1;
          do {
            exitg1 = 0;
            if (kstr < 6) {
              if (a_data[kstr] != b_cv[kstr]) {
                exitg1 = 1;
              } else {
                kstr++;
              }
            } else {
              b_bool = true;
              exitg1 = 1;
            }
          } while (exitg1 == 0);
        }

        return b_bool;
      }

      boolean_T g_strcmp(const char_T a_data[], const int32_T a_size[2])
      {
        static const char_T b_cv[6]{ 'p', 'o', 'i', 'n', 't', 's' };

        boolean_T b_bool;
        b_bool = false;
        if (a_size[1] == 6) {
          int32_T kstr;
          kstr = 0;
          int32_T exitg1;
          do {
            exitg1 = 0;
            if (kstr < 6) {
              if (a_data[kstr] != b_cv[kstr]) {
                exitg1 = 1;
              } else {
                kstr++;
              }
            } else {
              b_bool = true;
              exitg1 = 1;
            }
          } while (exitg1 == 0);
        }

        return b_bool;
      }

      boolean_T h_strcmp(const char_T a_data[], const int32_T a_size[2])
      {
        static const char_T b_cv[9]{ 'c', 'o', 'n', 't', 'r', 'a', 's', 't', 's'
        };

        boolean_T b_bool;
        b_bool = false;
        if (a_size[1] == 9) {
          int32_T kstr;
          kstr = 0;
          int32_T exitg1;
          do {
            exitg1 = 0;
            if (kstr < 9) {
              if (a_data[kstr] != b_cv[kstr]) {
                exitg1 = 1;
              } else {
                kstr++;
              }
            } else {
              b_bool = true;
              exitg1 = 1;
            }
          } while (exitg1 == 0);
        }

        return b_bool;
      }

      boolean_T i_strcmp(const char_T a_data[], const int32_T a_size[2])
      {
        static const char_T b_cv[13]{ 'a', 'i', 'r', '/', 's', 'u', 'b', 's',
          't', 'r', 'a', 't', 'e' };

        boolean_T b_bool;
        b_bool = false;
        if (a_size[1] == 13) {
          int32_T kstr;
          kstr = 0;
          int32_T exitg1;
          do {
            exitg1 = 0;
            if (kstr < 13) {
              if (cv[static_cast<uint8_T>(a_data[kstr]) & 127] != cv[
                  static_cast<int32_T>(b_cv[kstr])]) {
                exitg1 = 1;
              } else {
                kstr++;
              }
            } else {
              b_bool = true;
              exitg1 = 1;
            }
          } while (exitg1 == 0);
        }

        return b_bool;
      }

      boolean_T j_strcmp(const char_T a_data[], const int32_T a_size[2])
      {
        static const char_T b_cv[6]{ 'm', 'a', 't', 'l', 'a', 'b' };

        boolean_T b_bool;
        b_bool = false;
        if (a_size[1] == 6) {
          int32_T kstr;
          kstr = 0;
          int32_T exitg1;
          do {
            exitg1 = 0;
            if (kstr < 6) {
              if (a_data[kstr] != b_cv[kstr]) {
                exitg1 = 1;
              } else {
                kstr++;
              }
            } else {
              b_bool = true;
              exitg1 = 1;
            }
          } while (exitg1 == 0);
        }

        return b_bool;
      }

      boolean_T k_strcmp(const char_T a_data[], const int32_T a_size[2])
      {
        static const char_T b_cv[3]{ 'c', 'p', 'p' };

        boolean_T b_bool;
        b_bool = false;
        if (a_size[1] == 3) {
          int32_T kstr;
          kstr = 0;
          int32_T exitg1;
          do {
            exitg1 = 0;
            if (kstr < 3) {
              if (a_data[kstr] != b_cv[kstr]) {
                exitg1 = 1;
              } else {
                kstr++;
              }
            } else {
              b_bool = true;
              exitg1 = 1;
            }
          } while (exitg1 == 0);
        }

        return b_bool;
      }

      boolean_T l_strcmp(const char_T a_data[], const int32_T a_size[2])
      {
        static const char_T b_cv[6]{ 'p', 'y', 't', 'h', 'o', 'n' };

        boolean_T b_bool;
        b_bool = false;
        if (a_size[1] == 6) {
          int32_T kstr;
          kstr = 0;
          int32_T exitg1;
          do {
            exitg1 = 0;
            if (kstr < 6) {
              if (a_data[kstr] != b_cv[kstr]) {
                exitg1 = 1;
              } else {
                kstr++;
              }
            } else {
              b_bool = true;
              exitg1 = 1;
            }
          } while (exitg1 == 0);
        }

        return b_bool;
      }
    }
  }
}

// End of code generation (strcmp.cpp)
