//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// optimalEllipsoids.cpp
//
// Code generation for function 'optimalEllipsoids'
//

// Include files
#include "optimalEllipsoids.h"
#include "RATMain_data.h"
#include "all.h"
#include "calcEllipsoid.h"
#include "rt_nonfinite.h"
#include "splitEllipsoid.h"
#include "validate_print_arguments.h"
#include "coder_array.h"
#include <stdio.h>

// Function Declarations
namespace RAT
{
  static void d_binary_expand_op(boolean_T in1_data[], int32_T in1_size[2],
    const real_T in2_data[], const int32_T in2_size[2], const real_T in3_data[],
    const int32_T in3_size[2], const ::coder::array<real_T, 2U> &in4);
}

// Function Definitions
namespace RAT
{
  static void d_binary_expand_op(boolean_T in1_data[], int32_T in1_size[2],
    const real_T in2_data[], const int32_T in2_size[2], const real_T in3_data[],
    const int32_T in3_size[2], const ::coder::array<real_T, 2U> &in4)
  {
    int32_T b_in2_size[2];
    int32_T aux_0_1;
    int32_T aux_1_1;
    int32_T aux_2_1;
    int32_T loop_ub;
    int32_T stride_0_0;
    int32_T stride_0_1;
    int32_T stride_1_0;
    int32_T stride_1_1;
    int32_T stride_2_0;
    int32_T stride_2_1;
    boolean_T b_in2_data;
    if (in4.size(0) == 1) {
      if (in3_size[0] == 1) {
        b_in2_size[0] = in2_size[0];
      } else {
        b_in2_size[0] = in3_size[0];
      }
    } else {
      b_in2_size[0] = in4.size(0);
    }

    if (in4.size(1) == 1) {
      if (in3_size[1] == 1) {
        b_in2_size[1] = in2_size[1];
      } else {
        b_in2_size[1] = in3_size[1];
      }
    } else {
      b_in2_size[1] = in4.size(1);
    }

    stride_0_0 = (in2_size[0] != 1);
    stride_0_1 = (in2_size[1] != 1);
    stride_1_0 = (in3_size[0] != 1);
    stride_1_1 = (in3_size[1] != 1);
    stride_2_0 = (in4.size(0) != 1);
    stride_2_1 = (in4.size(1) != 1);
    aux_0_1 = 0;
    aux_1_1 = 0;
    aux_2_1 = 0;
    if (in4.size(1) == 1) {
      if (in3_size[1] == 1) {
        loop_ub = in2_size[1];
      } else {
        loop_ub = in3_size[1];
      }
    } else {
      loop_ub = in4.size(1);
    }

    for (int32_T i{0}; i < loop_ub; i++) {
      int32_T b_loop_ub;
      int32_T i1;
      i1 = in4.size(0);
      b_loop_ub = in3_size[0];
      if (i1 == 1) {
        if (b_loop_ub == 1) {
          b_loop_ub = in2_size[0];
        }
      } else {
        b_loop_ub = i1;
      }

      for (i1 = 0; i1 < b_loop_ub; i1++) {
        b_in2_data = (in2_data[i1 * stride_0_0 + in2_size[0] * aux_0_1] +
                      in3_data[i1 * stride_1_0 + in3_size[0] * aux_1_1] < in4[i1
                      * stride_2_0 + in4.size(0) * aux_2_1]);
      }

      aux_2_1 += stride_2_1;
      aux_1_1 += stride_1_1;
      aux_0_1 += stride_0_1;
    }

    coder::all((const boolean_T *)&b_in2_data, b_in2_size, in1_data, in1_size);
  }

  void optimalEllipsoids(const ::coder::array<real_T, 2U> &u, real_T VS, ::coder::
    array<real_T, 2U> &Bs, ::coder::array<real_T, 2U> &mus, ::coder::array<
    real_T, 2U> &VEs, ::coder::array<real_T, 2U> &ns)
  {
    ::coder::array<real_T, 2U> B1;
    ::coder::array<real_T, 2U> B2;
    ::coder::array<real_T, 2U> VE1;
    ::coder::array<real_T, 2U> VE2;
    ::coder::array<real_T, 2U> mu;
    ::coder::array<real_T, 2U> mu1;
    ::coder::array<real_T, 2U> mu2;
    ::coder::array<real_T, 2U> n1;
    ::coder::array<real_T, 2U> n2;
    ::coder::array<real_T, 2U> u1;
    ::coder::array<real_T, 2U> u2;
    real_T VE1_data;
    real_T VE2_data;
    real_T flag;
    real_T nosplit;
    int32_T VE1_size[2];
    int32_T VE2_size[2];
    int32_T b_VE1_size[2];
    int32_T tmp_size[2];
    boolean_T empty_non_axis_sizes;

    //  function [Bs, mus, VEs, ns] = optimalEllipsoids(u, VS)
    //
    //  This function attempts to optimally partition the multi-dimensional
    //  samples u (uniformly distributed within the sample volume VS), into
    //  a set of subclusters enclosed by bounding ellipsoids.  The algorithm
    //  is based on Algorithm 1 of the MULTINEST paper by Feroz, Hobson,
    //  and Bridges, MNRAS, 398, 1601-1614 (2009).
    //
    //  Output:
    //    Bs:  an array of bounding matrices for the ellipsoids enclosing
    //         the subclusters, scaled to have at least the minimum volume
    //         required by the subclusters. ( (K x ndims) x ndims )
    //    mus: an array of centroids for the bounding ellipsoids (K x ndims)
    //    VEs: an array of volumes for the bounding ellipsoids   (K x 1)
    //    ns:  an array containing the number of points for each subcluster (K x 1)
    //
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //  number of samples in multi-dimensional space
    //  number of dimensions
    //  calculate bounding matrix, etc. for bounding ellipsoid associated
    //  with the original set of points u
    VEs.reserve(1);
    calcEllipsoid(u, VS, Bs, mu, (real_T *)VEs.data(), VEs.size(), &flag);

    //  attempt to split u into two subclusters
    splitEllipsoid(u, VS, u1, u2, (real_T *)&VE1_data, VE1_size, (real_T *)
                   &VE2_data, VE2_size, &nosplit);
    if ((nosplit != 0.0) || (static_cast<uint32_T>(u1.size(0)) <
         static_cast<uint32_T>(u.size(1)) + 1U) || (static_cast<uint32_T>
         (u2.size(0)) < static_cast<uint32_T>(u.size(1)) + 1U)) {
      int32_T input_sizes_idx_0;

      //  couldn't split the cluster
      mus.set_size(mu.size(0), mu.size(1));
      input_sizes_idx_0 = mu.size(1);
      for (int32_T i{0}; i < input_sizes_idx_0; i++) {
        int32_T loop_ub;
        loop_ub = mu.size(0);
        for (int32_T i1{0}; i1 < loop_ub; i1++) {
          mus[mus.size(0) * i] = mu[mu.size(0) * i];
        }
      }

      ns.set_size(1, 1);
      ns[0] = u.size(0);
    } else {
      int32_T validatedHoleFilling[3];
      int32_T i;
      int32_T i1;
      int32_T input_sizes_idx_0;
      int32_T loop_ub;
      boolean_T b_VE1_data;
      boolean_T guard1;

      //  check if we should keep the partitioning of S
      // if (all(VE1 + VE2 < VE) || all(VE > 2*VS))
      if (VE1_size[0] == 1) {
        i = VE2_size[0];
      } else {
        i = VE1_size[0];
      }

      if (VE1_size[1] == 1) {
        i1 = VE2_size[1];
      } else {
        i1 = VE1_size[1];
      }

      if ((VE1_size[0] == VE2_size[0]) && (VE1_size[1] == VE2_size[1]) && (i ==
           VEs.size(0)) && (i1 == VEs.size(1))) {
        b_VE1_size[0] = VE1_size[0];
        b_VE1_size[1] = VE1_size[1];
        input_sizes_idx_0 = VE1_size[1];
        for (i = 0; i < input_sizes_idx_0; i++) {
          loop_ub = VE1_size[0];
          for (i1 = 0; i1 < loop_ub; i1++) {
            b_VE1_data = (VE1_data + VE2_data < VEs[0]);
          }
        }

        coder::all((const boolean_T *)&b_VE1_data, b_VE1_size, (boolean_T *)
                   &empty_non_axis_sizes, tmp_size);
      } else {
        d_binary_expand_op((boolean_T *)&empty_non_axis_sizes, tmp_size, (const
          real_T *)&VE1_data, VE1_size, (const real_T *)&VE2_data, VE2_size, VEs);
      }

      guard1 = false;
      if (coder::b_all((const boolean_T *)&empty_non_axis_sizes, tmp_size)) {
        guard1 = true;
      } else {
        flag = 2.0 * VS;
        b_VE1_size[0] = VEs.size(0);
        b_VE1_size[1] = VEs.size(1);
        input_sizes_idx_0 = VEs.size(1);
        for (i = 0; i < input_sizes_idx_0; i++) {
          loop_ub = VEs.size(0);
          for (i1 = 0; i1 < loop_ub; i1++) {
            b_VE1_data = (VEs[i1 + VEs.size(0) * i] > flag);
          }
        }

        coder::all((const boolean_T *)&b_VE1_data, b_VE1_size, (boolean_T *)
                   &empty_non_axis_sizes, tmp_size);
        if (coder::b_all((const boolean_T *)&empty_non_axis_sizes, tmp_size)) {
          guard1 = true;
        } else {
          if (DEBUG != 0.0) {
            coder::internal::validate_print_arguments(u.size(0), u1.size(0),
              u2.size(0), validatedHoleFilling);
            printf("PARTITION REJECTED: N=%d doesnt split into n1=%d and n2=%d\n",
                   validatedHoleFilling[0], validatedHoleFilling[1],
                   validatedHoleFilling[2]);
            fflush(stdout);
          }

          mus.set_size(mu.size(0), mu.size(1));
          input_sizes_idx_0 = mu.size(1);
          for (i = 0; i < input_sizes_idx_0; i++) {
            loop_ub = mu.size(0);
            for (i1 = 0; i1 < loop_ub; i1++) {
              mus[mus.size(0) * i] = mu[mu.size(0) * i];
            }
          }

          ns.set_size(1, 1);
          ns[0] = u.size(0);
        }
      }

      if (guard1) {
        int32_T result;
        int32_T sizes_idx_0;
        int16_T b_input_sizes_idx_0;
        if (DEBUG != 0.0) {
          coder::internal::validate_print_arguments(u.size(0), u1.size(0),
            u2.size(0), validatedHoleFilling);
          printf("PARTITION ACCEPTED: N=%d splits to n1=%d, n2=%d\n",
                 validatedHoleFilling[0], validatedHoleFilling[1],
                 validatedHoleFilling[2]);
          fflush(stdout);
        }

        optimalEllipsoids(u1, static_cast<real_T>(u1.size(0)) * VS /
                          static_cast<real_T>(u.size(0)), B1, mu1, VE1, n1);
        optimalEllipsoids(u2, static_cast<real_T>(u2.size(0)) * VS /
                          static_cast<real_T>(u.size(0)), B2, mu2, VE2, n2);
        if ((B1.size(0) != 0) && (B1.size(1) != 0)) {
          result = B1.size(1);
        } else if ((B2.size(0) != 0) && (B2.size(1) != 0)) {
          result = B2.size(1);
        } else {
          result = B1.size(1);
          if (B2.size(1) > B1.size(1)) {
            result = B2.size(1);
          }
        }

        empty_non_axis_sizes = (result == 0);
        if (empty_non_axis_sizes || ((B1.size(0) != 0) && (B1.size(1) != 0))) {
          input_sizes_idx_0 = B1.size(0);
        } else {
          input_sizes_idx_0 = 0;
        }

        if (empty_non_axis_sizes || ((B2.size(0) != 0) && (B2.size(1) != 0))) {
          sizes_idx_0 = B2.size(0);
        } else {
          sizes_idx_0 = 0;
        }

        Bs.set_size(input_sizes_idx_0 + sizes_idx_0, result);
        for (i = 0; i < result; i++) {
          for (i1 = 0; i1 < input_sizes_idx_0; i1++) {
            Bs[i1 + Bs.size(0) * i] = B1[i1 + input_sizes_idx_0 * i];
          }
        }

        for (i = 0; i < result; i++) {
          for (i1 = 0; i1 < sizes_idx_0; i1++) {
            Bs[(i1 + input_sizes_idx_0) + Bs.size(0) * i] = B2[i1 + sizes_idx_0 *
              i];
          }
        }

        if ((mu1.size(0) != 0) && (mu1.size(1) != 0)) {
          result = mu1.size(1);
        } else if ((mu2.size(0) != 0) && (mu2.size(1) != 0)) {
          result = mu2.size(1);
        } else {
          result = mu1.size(1);
          if (mu2.size(1) > mu1.size(1)) {
            result = mu2.size(1);
          }
        }

        empty_non_axis_sizes = (result == 0);
        if (empty_non_axis_sizes || ((mu1.size(0) != 0) && (mu1.size(1) != 0)))
        {
          b_input_sizes_idx_0 = static_cast<int16_T>(mu1.size(0));
        } else {
          b_input_sizes_idx_0 = 0;
        }

        if (empty_non_axis_sizes || ((mu2.size(0) != 0) && (mu2.size(1) != 0)))
        {
          sizes_idx_0 = mu2.size(0);
        } else {
          sizes_idx_0 = 0;
        }

        loop_ub = b_input_sizes_idx_0;
        mus.set_size(b_input_sizes_idx_0 + sizes_idx_0, result);
        for (i = 0; i < result; i++) {
          for (i1 = 0; i1 < loop_ub; i1++) {
            mus[i1 + mus.size(0) * i] = mu1[i1 + b_input_sizes_idx_0 * i];
          }
        }

        for (i = 0; i < result; i++) {
          for (i1 = 0; i1 < sizes_idx_0; i1++) {
            mus[(i1 + b_input_sizes_idx_0) + mus.size(0) * i] = mu2[i1 +
              sizes_idx_0 * i];
          }
        }

        if ((VE1.size(0) != 0) && (VE1.size(1) != 0)) {
          result = VE1.size(1);
        } else if ((VE2.size(0) != 0) && (VE2.size(1) != 0)) {
          result = VE2.size(1);
        } else {
          result = VE1.size(1);
          if (VE2.size(1) > VE1.size(1)) {
            result = VE2.size(1);
          }
        }

        empty_non_axis_sizes = (result == 0);
        if (empty_non_axis_sizes || ((VE1.size(0) != 0) && (VE1.size(1) != 0)))
        {
          input_sizes_idx_0 = VE1.size(0);
        } else {
          input_sizes_idx_0 = 0;
        }

        if (empty_non_axis_sizes || ((VE2.size(0) != 0) && (VE2.size(1) != 0)))
        {
          sizes_idx_0 = VE2.size(0);
        } else {
          sizes_idx_0 = 0;
        }

        VEs.set_size(input_sizes_idx_0 + sizes_idx_0, result);
        for (i = 0; i < result; i++) {
          for (i1 = 0; i1 < input_sizes_idx_0; i1++) {
            VEs[i1 + VEs.size(0) * i] = VE1[i1 + input_sizes_idx_0 * i];
          }
        }

        for (i = 0; i < result; i++) {
          for (i1 = 0; i1 < sizes_idx_0; i1++) {
            VEs[(i1 + input_sizes_idx_0) + VEs.size(0) * i] = VE2[i1 +
              sizes_idx_0 * i];
          }
        }

        if ((n1.size(0) != 0) && (n1.size(1) != 0)) {
          result = n1.size(1);
        } else if ((n2.size(0) != 0) && (n2.size(1) != 0)) {
          result = n2.size(1);
        } else {
          result = (n1.size(1) > 0);
          if (n2.size(1) > result) {
            result = 1;
          }
        }

        empty_non_axis_sizes = (result == 0);
        if (empty_non_axis_sizes || ((n1.size(0) != 0) && (n1.size(1) != 0))) {
          b_input_sizes_idx_0 = static_cast<int16_T>(n1.size(0));
        } else {
          b_input_sizes_idx_0 = 0;
        }

        if (empty_non_axis_sizes || ((n2.size(0) != 0) && (n2.size(1) != 0))) {
          sizes_idx_0 = n2.size(0);
        } else {
          sizes_idx_0 = 0;
        }

        loop_ub = b_input_sizes_idx_0;
        ns.set_size(b_input_sizes_idx_0 + sizes_idx_0, result);
        for (i = 0; i < result; i++) {
          for (i1 = 0; i1 < loop_ub; i1++) {
            ns[i1 + ns.size(0) * i] = n1[i1 + b_input_sizes_idx_0 * i];
          }
        }

        for (i = 0; i < result; i++) {
          for (i1 = 0; i1 < sizes_idx_0; i1++) {
            ns[(i1 + b_input_sizes_idx_0) + ns.size(0) * i] = n2[i1 +
              sizes_idx_0 * i];
          }
        }
      }
    }
  }
}

// End of code generation (optimalEllipsoids.cpp)
