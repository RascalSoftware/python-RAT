//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// abelesSingle.cpp
//
// Code generation for function 'abelesSingle'
//

// Include files
#include "abelesSingle.h"
#include "exp.h"
#include "reflectivityCalculation_data.h"
#include "reflectivityCalculation_rtwutil.h"
#include "rt_nonfinite.h"
#include "sqrt.h"
#include "coder_array.h"
#include <cmath>

// Function Definitions
namespace RAT
{
  void abelesSingle(const ::coder::array<real_T, 1U> &q, real_T N, const ::coder::
                    array<real_T, 1U> &layers_thick, const ::coder::array<
                    creal_T, 1U> &layers_rho, const ::coder::array<real_T, 1U>
                    &layers_sig, ::coder::array<real_T, 1U> &ref)
  {
    creal_T M_n[2][2];
    creal_T M_res[2][2];
    creal_T M_tot[2][2];
    creal_T M_n_tmp;
    creal_T k1;
    creal_T knp1;
    real_T kn_ptr_im;
    real_T kn_ptr_re;
    int32_T i;
    int32_T loop_ub;

    //  New Matlab version of reflectivity
    //  with complex rho...
    //  Pre-allocation
    M_tot[0][0].re = 0.0;
    M_tot[0][0].im = 0.0;
    M_res[0][0].re = 0.0;
    M_res[0][0].im = 0.0;
    M_tot[0][1].re = 0.0;
    M_tot[0][1].im = 0.0;
    M_res[0][1].re = 0.0;
    M_res[0][1].im = 0.0;
    M_tot[1][0].re = 0.0;
    M_tot[1][0].im = 0.0;
    M_tot[1][1].re = 0.0;
    M_tot[1][1].im = 0.0;
    kn_ptr_re = 0.0;
    kn_ptr_im = 0.0;
    ref.set_size(q.size(0));
    loop_ub = q.size(0);
    for (i = 0; i < loop_ub; i++) {
      ref[i] = 0.0;
    }

    i = q.size(0);
    for (int32_T points{0}; points < i; points++) {
      real_T M_n_tmp_tmp;
      real_T brm;
      real_T bulk_in_SLD_im;
      real_T bulk_in_SLD_re;
      real_T k0;
      real_T re;
      bulk_in_SLD_re = layers_rho[0].re;
      bulk_in_SLD_im = layers_rho[0].im + 1.0E-30;
      k0 = q[points] / 2.0;
      loop_ub = static_cast<int32_T>(N - 1.0);
      for (int32_T n{0}; n < loop_ub; n++) {
        if (static_cast<uint32_T>(n) + 1U == 1U) {
          real_T ar;
          real_T im;
          real_T r01_im;
          real_T r01_re;

          //  Find k1..
          k1.re = k0 * k0 - 12.566370614359172 * (layers_rho[1].re -
            bulk_in_SLD_re);
          k1.im = 0.0 - 12.566370614359172 * (layers_rho[1].im - bulk_in_SLD_im);
          coder::internal::scalar::b_sqrt(&k1);

          //  Find r01
          M_n_tmp_tmp = layers_sig[1] * layers_sig[1];
          M_n_tmp.re = M_n_tmp_tmp * (k0 * (-2.0 * k1.re));
          M_n_tmp.im = M_n_tmp_tmp * (k0 * (-2.0 * k1.im));
          coder::b_exp(&M_n_tmp);
          ar = k0 - k1.re;
          im = k0 + k1.re;
          if (k1.im == 0.0) {
            im = ar / im;
            re = 0.0;
          } else if (im == 0.0) {
            if (ar == 0.0) {
              im = (0.0 - k1.im) / k1.im;
              re = 0.0;
            } else if (0.0 - k1.im == 0.0) {
              im = 0.0;
              re = -(ar / k1.im);
            } else {
              im = (0.0 - k1.im) / k1.im;
              re = -(ar / k1.im);
            }
          } else {
            brm = std::abs(im);
            re = std::abs(k1.im);
            if (brm > re) {
              M_n_tmp_tmp = k1.im / im;
              re = im + M_n_tmp_tmp * k1.im;
              im = (ar + M_n_tmp_tmp * (0.0 - k1.im)) / re;
              re = ((0.0 - k1.im) - M_n_tmp_tmp * ar) / re;
            } else if (re == brm) {
              if (im > 0.0) {
                M_n_tmp_tmp = 0.5;
              } else {
                M_n_tmp_tmp = -0.5;
              }

              if (k1.im > 0.0) {
                re = 0.5;
              } else {
                re = -0.5;
              }

              im = (ar * M_n_tmp_tmp + (0.0 - k1.im) * re) / brm;
              re = ((0.0 - k1.im) * M_n_tmp_tmp - ar * re) / brm;
            } else {
              M_n_tmp_tmp = im / k1.im;
              re = k1.im + M_n_tmp_tmp * im;
              im = (M_n_tmp_tmp * ar + (0.0 - k1.im)) / re;
              re = (M_n_tmp_tmp * (0.0 - k1.im) - ar) / re;
            }
          }

          r01_re = im * M_n_tmp.re - re * M_n_tmp.im;
          r01_im = im * M_n_tmp.im + re * M_n_tmp.re;

          //  Generate the M1 matrix:
          M_tot[0][0].re = 1.0;
          M_tot[0][0].im = 0.0;
          M_tot[1][0].re = r01_re;
          M_tot[1][0].im = r01_im;
          M_tot[0][1].re = r01_re;
          M_tot[0][1].im = r01_im;
          M_tot[1][1].re = 1.0;
          M_tot[1][1].im = 0.0;
          kn_ptr_re = k1.re;
          kn_ptr_im = k1.im;
        } else {
          real_T ai;
          real_T ar;
          real_T b_kn_ptr_re;
          real_T beta_im;
          real_T beta_re;
          real_T bi;
          real_T d;
          real_T d1;
          real_T im;
          real_T r_n_np1_im;
          real_T r_n_np1_re;

          //  Find kn and k_n+1 (ex. k1 and k2 for n=1): _/
          knp1.re = k0 * k0 - 12.566370614359172 * (layers_rho[n + 1].re -
            bulk_in_SLD_re);
          knp1.im = 0.0 - 12.566370614359172 * (layers_rho[n + 1].im -
            bulk_in_SLD_im);
          coder::internal::scalar::b_sqrt(&knp1);

          //  Find r_n,n+1:
          re = -2.0 * kn_ptr_re;
          im = -2.0 * kn_ptr_im;
          M_n_tmp_tmp = layers_sig[n + 1];
          M_n_tmp_tmp *= M_n_tmp_tmp;
          M_n_tmp.re = M_n_tmp_tmp * (re * knp1.re - im * knp1.im);
          M_n_tmp.im = M_n_tmp_tmp * (re * knp1.im + im * knp1.re);
          coder::b_exp(&M_n_tmp);
          ar = kn_ptr_re - knp1.re;
          ai = kn_ptr_im - knp1.im;
          im = kn_ptr_re + knp1.re;
          bi = kn_ptr_im + knp1.im;
          if (bi == 0.0) {
            if (ai == 0.0) {
              b_kn_ptr_re = ar / im;
              re = 0.0;
            } else if (ar == 0.0) {
              b_kn_ptr_re = 0.0;
              re = ai / im;
            } else {
              b_kn_ptr_re = ar / im;
              re = ai / im;
            }
          } else if (im == 0.0) {
            if (ar == 0.0) {
              b_kn_ptr_re = ai / bi;
              re = 0.0;
            } else if (ai == 0.0) {
              b_kn_ptr_re = 0.0;
              re = -(ar / bi);
            } else {
              b_kn_ptr_re = ai / bi;
              re = -(ar / bi);
            }
          } else {
            brm = std::abs(im);
            re = std::abs(bi);
            if (brm > re) {
              M_n_tmp_tmp = bi / im;
              re = im + M_n_tmp_tmp * bi;
              b_kn_ptr_re = (ar + M_n_tmp_tmp * ai) / re;
              re = (ai - M_n_tmp_tmp * ar) / re;
            } else if (re == brm) {
              if (im > 0.0) {
                M_n_tmp_tmp = 0.5;
              } else {
                M_n_tmp_tmp = -0.5;
              }

              if (bi > 0.0) {
                re = 0.5;
              } else {
                re = -0.5;
              }

              b_kn_ptr_re = (ar * M_n_tmp_tmp + ai * re) / brm;
              re = (ai * M_n_tmp_tmp - ar * re) / brm;
            } else {
              M_n_tmp_tmp = im / bi;
              re = bi + M_n_tmp_tmp * im;
              b_kn_ptr_re = (M_n_tmp_tmp * ar + ai) / re;
              re = (M_n_tmp_tmp * ai - ar) / re;
            }
          }

          r_n_np1_re = b_kn_ptr_re * M_n_tmp.re - re * M_n_tmp.im;
          r_n_np1_im = b_kn_ptr_re * M_n_tmp.im + re * M_n_tmp.re;

          //  Find the Phase Factor = (k_n * d_n)
          kn_ptr_re *= layers_thick[n];
          kn_ptr_im *= layers_thick[n];
          beta_re = kn_ptr_re * 0.0 - kn_ptr_im;
          beta_im = kn_ptr_re + kn_ptr_im * 0.0;

          //  Create the M_n matrix: _/
          M_n_tmp.re = beta_re;
          M_n_tmp.im = beta_im;
          coder::b_exp(&M_n_tmp);
          M_n[0][0] = M_n_tmp;
          M_n[1][0].re = r_n_np1_re * M_n_tmp.re - r_n_np1_im * M_n_tmp.im;
          M_n[1][0].im = r_n_np1_re * M_n_tmp.im + r_n_np1_im * M_n_tmp.re;
          M_n_tmp.re = -beta_re;
          M_n_tmp.im = -beta_im;
          coder::b_exp(&M_n_tmp);

          //  Multiply the matrices
          ar = M_n[0][0].re;
          d = M_n[0][0].im;
          d1 = r_n_np1_re * M_n_tmp.re - r_n_np1_im * M_n_tmp.im;
          re = r_n_np1_re * M_n_tmp.im + r_n_np1_im * M_n_tmp.re;
          im = M_n[1][0].re;
          M_n_tmp_tmp = M_n[1][0].im;
          for (int32_T i1{0}; i1 < 2; i1++) {
            bi = M_tot[0][i1].re;
            brm = M_tot[0][i1].im;
            ai = M_tot[1][i1].re;
            b_kn_ptr_re = M_tot[1][i1].im;
            M_res[0][i1].re = (bi * ar - brm * d) + (ai * d1 - b_kn_ptr_re * re);
            M_res[0][i1].im = (bi * d + brm * ar) + (ai * re + b_kn_ptr_re * d1);
            M_res[1][i1].re = (bi * im - brm * M_n_tmp_tmp) + (ai * M_n_tmp.re -
              b_kn_ptr_re * M_n_tmp.im);
            M_res[1][i1].im = (bi * M_n_tmp_tmp + brm * im) + (ai * M_n_tmp.im +
              b_kn_ptr_re * M_n_tmp.re);
          }

          //  Reassign the values back to M_tot:
          M_tot[0][0] = M_res[0][0];
          M_tot[0][1] = M_res[0][1];
          M_tot[1][0] = M_res[1][0];
          M_tot[1][1] = M_res[1][1];

          //  Point to k_n+1 and sld_n+1 via kn_ptr sld_n_ptr:
          kn_ptr_re = knp1.re;
          kn_ptr_im = knp1.im;
        }
      }

      if (M_res[0][0].im == 0.0) {
        if (M_res[0][1].im == 0.0) {
          M_n_tmp.re = M_res[0][1].re / M_res[0][0].re;
          M_n_tmp.im = 0.0;
        } else if (M_res[0][1].re == 0.0) {
          M_n_tmp.re = 0.0;
          M_n_tmp.im = M_res[0][1].im / M_res[0][0].re;
        } else {
          M_n_tmp.re = M_res[0][1].re / M_res[0][0].re;
          M_n_tmp.im = M_res[0][1].im / M_res[0][0].re;
        }
      } else if (M_res[0][0].re == 0.0) {
        if (M_res[0][1].re == 0.0) {
          M_n_tmp.re = M_res[0][1].im / M_res[0][0].im;
          M_n_tmp.im = 0.0;
        } else if (M_res[0][1].im == 0.0) {
          M_n_tmp.re = 0.0;
          M_n_tmp.im = -(M_res[0][1].re / M_res[0][0].im);
        } else {
          M_n_tmp.re = M_res[0][1].im / M_res[0][0].im;
          M_n_tmp.im = -(M_res[0][1].re / M_res[0][0].im);
        }
      } else {
        brm = std::abs(M_res[0][0].re);
        re = std::abs(M_res[0][0].im);
        if (brm > re) {
          M_n_tmp_tmp = M_res[0][0].im / M_res[0][0].re;
          re = M_res[0][0].re + M_n_tmp_tmp * M_res[0][0].im;
          M_n_tmp.re = (M_res[0][1].re + M_n_tmp_tmp * M_res[0][1].im) / re;
          M_n_tmp.im = (M_res[0][1].im - M_n_tmp_tmp * M_res[0][1].re) / re;
        } else if (re == brm) {
          if (M_res[0][0].re > 0.0) {
            M_n_tmp_tmp = 0.5;
          } else {
            M_n_tmp_tmp = -0.5;
          }

          if (M_res[0][0].im > 0.0) {
            re = 0.5;
          } else {
            re = -0.5;
          }

          M_n_tmp.re = (M_res[0][1].re * M_n_tmp_tmp + M_res[0][1].im * re) /
            brm;
          M_n_tmp.im = (M_res[0][1].im * M_n_tmp_tmp - M_res[0][1].re * re) /
            brm;
        } else {
          M_n_tmp_tmp = M_res[0][0].re / M_res[0][0].im;
          re = M_res[0][0].im + M_n_tmp_tmp * M_res[0][0].re;
          M_n_tmp.re = (M_n_tmp_tmp * M_res[0][1].re + M_res[0][1].im) / re;
          M_n_tmp.im = (M_n_tmp_tmp * M_res[0][1].im - M_res[0][1].re) / re;
        }
      }

      re = rt_hypotd_snf(M_n_tmp.re, M_n_tmp.im);
      ref[points] = re * re;
    }
  }
}

// End of code generation (abelesSingle.cpp)
