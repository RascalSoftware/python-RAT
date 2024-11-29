import numpy as np

import RATapi as RAT


def two_contrast_example():
    # Two contrast example for the user guide
    problem = RAT.Project(name="DSPC monolayers")
    parameters = [
        RAT.models.Parameter(name="Tails Thickness", min=10, value=20, max=30, fit=True),
        RAT.models.Parameter(name="Heads Thickness", min=3, value=11, max=16, fit=True),
        RAT.models.Parameter(name="Tails Roughness", min=2, value=5, max=9, fit=True),
        RAT.models.Parameter(name="Heads Roughness", min=2, value=5, max=9, fit=True),
        RAT.models.Parameter(name="Deuterated Tails SLD", min=4e-6, value=6e-6, max=2e-5, fit=True),
        RAT.models.Parameter(name="Hydrogenated Tails SLD", min=-0.6e-6, value=-0.4e-6, max=0, fit=True),
        RAT.models.Parameter(name="Deuterated Heads SLD", min=1e-6, value=3e-6, max=8e-6, fit=True),
        RAT.models.Parameter(name="Hydrogenated Heads SLD", min=0.1e-6, value=1.4e-6, max=3e-6, fit=True),
        RAT.models.Parameter(name="Heads Hydration", min=0, value=0.3, max=0.5, fit=True),
    ]

    problem.parameters.extend(parameters)
    H_Heads = RAT.models.Layer(
        name="Hydrogenated Heads",
        thickness="Heads Thickness",
        SLD="Hydrogenated Heads SLD",
        roughness="Heads Roughness",
        hydration="Heads Hydration",
        hydrate_with="bulk out",
    )

    D_Heads = RAT.models.Layer(
        name="Deuterated Heads",
        thickness="Heads Thickness",
        SLD="Deuterated Heads SLD",
        roughness="Heads Roughness",
        hydration="Heads Hydration",
        hydrate_with="bulk out",
    )

    D_Tails = RAT.models.Layer(
        name="Deuterated Tails", thickness="Tails Thickness", SLD="Deuterated Tails SLD", roughness="Tails Roughness"
    )

    H_Tails = RAT.models.Layer(
        name="Hydrogenated Tails",
        thickness="Tails Thickness",
        SLD="Hydrogenated Tails SLD",
        roughness="Tails Roughness",
    )

    problem.layers.extend([H_Heads, D_Heads, H_Tails, D_Tails])

    problem.background_parameters.set_fields(0, name="Backs Value ACMW")
    problem.background_parameters.set_fields(0, value=5.5e-6)
    problem.background_parameters.append(name="Backs Value D2O", min=1e-8, value=2.8e-6, max=1e-5)

    problem.backgrounds.append(name="Background D2O", type="constant", value_1="Backs Value D2O")
    problem.backgrounds.set_fields(0, name="Background ACMW", value_1="Backs Value ACMW")

    problem.bulk_out.append(name="SLD ACMW", min=-0.6e-6, value=-0.56e-6, max=-0.3e-6, fit=True)

    d13acmw20 = np.array(
        [
            [5.1793e-02, 1.8087e-05, 7.9683e-07],
            [5.4383e-02, 1.4582e-05, 6.3691e-07],
            [5.7102e-02, 1.3621e-05, 5.5407e-07],
            [5.9957e-02, 1.2409e-05, 4.8014e-07],
            [6.2955e-02, 1.0992e-05, 4.1824e-07],
            [6.6103e-02, 1.0619e-05, 3.7541e-07],
            [6.9408e-02, 1.0162e-05, 3.4945e-07],
            [7.2878e-02, 1.0716e-05, 3.3763e-07],
            [7.6522e-02, 8.3468e-06, 2.8040e-07],
            [8.0348e-02, 7.5250e-06, 2.5702e-07],
            [8.4365e-02, 6.4395e-06, 2.2528e-07],
            [8.8584e-02, 6.0544e-06, 2.0545e-07],
            [9.3013e-02, 5.9517e-06, 1.9417e-07],
            [9.7664e-02, 5.6433e-06, 1.8108e-07],
            [1.0255e-01, 4.8172e-06, 1.6076e-07],
            [1.0767e-01, 4.8066e-06, 1.5520e-07],
            [1.1306e-01, 4.1559e-06, 1.4115e-07],
            [1.1871e-01, 4.1418e-06, 1.3926e-07],
            [1.2465e-01, 3.4580e-06, 1.2273e-07],
            [1.3088e-01, 3.2376e-06, 1.1791e-07],
            [1.3742e-01, 3.0976e-06, 1.1948e-07],
            [1.4429e-01, 2.7478e-06, 1.1518e-07],
            [1.5151e-01, 2.5492e-06, 1.1573e-07],
            [1.5908e-01, 2.7500e-06, 1.2812e-07],
            [1.6704e-01, 2.1813e-06, 2.0450e-07],
            [1.7539e-01, 1.8609e-06, 5.4900e-07],
            [1.8416e-01, 1.9405e-06, 1.5660e-07],
            [1.9337e-01, 1.7421e-06, 2.3280e-07],
            [2.0304e-01, 1.8050e-06, 3.9820e-07],
            [2.1319e-01, 1.5801e-06, 1.4110e-07],
            [2.2385e-01, 1.6724e-06, 7.7900e-08],
            [2.3504e-01, 1.4150e-06, 4.0820e-07],
            [2.4679e-01, 1.4340e-06, 5.3180e-07],
            [2.5913e-01, 1.3102e-06, 2.6000e-07],
            [2.7209e-01, 1.3702e-06, 2.8540e-07],
            [2.8569e-01, 1.2468e-06, 2.3230e-07],
            [2.9998e-01, 1.2956e-06, 5.3240e-07],
            [3.1497e-01, 1.2947e-06, 3.9940e-07],
            [3.3072e-01, 1.2488e-06, 2.1390e-07],
            [3.4726e-01, 1.4620e-06, 3.3640e-07],
            [3.6462e-01, 1.3056e-06, 2.1600e-07],
            [3.8285e-01, 1.4553e-06, 2.3170e-07],
            [4.0200e-01, 1.1579e-06, 2.6740e-07],
            [4.2210e-01, 1.1753e-06, 3.0940e-07],
            [4.4320e-01, 1.0066e-06, 5.2040e-07],
            [4.6536e-01, 1.1043e-06, 3.1870e-07],
            [4.8863e-01, 1.2969e-06, 4.1670e-07],
            [5.1306e-01, 1.2495e-06, 2.0890e-07],
            [5.3871e-01, 1.3898e-06, 4.7560e-07],
            [5.6565e-01, 1.1225e-06, 3.0470e-07],
            [5.8877e-01, 8.3346e-07, 3.8944e-07],
        ]
    )

    d70d2o20 = np.array(
        [
            [5.1793e-02, 1.4943e-04, 3.2517e-06],
            [5.4383e-02, 1.1882e-04, 2.5846e-06],
            [5.7102e-02, 9.2164e-05, 2.0507e-06],
            [5.9957e-02, 7.7813e-05, 1.7070e-06],
            [6.2955e-02, 6.1143e-05, 1.3983e-06],
            [6.6103e-02, 4.8033e-05, 1.1343e-06],
            [6.9408e-02, 4.1379e-05, 1.0006e-06],
            [7.2878e-02, 3.5090e-05, 8.6919e-07],
            [7.6522e-02, 3.0350e-05, 7.5890e-07],
            [8.0348e-02, 2.4115e-05, 6.5226e-07],
            [8.4365e-02, 2.0755e-05, 5.7445e-07],
            [8.8584e-02, 1.7500e-05, 4.9617e-07],
            [9.3013e-02, 1.5011e-05, 4.3754e-07],
            [9.7664e-02, 1.3632e-05, 3.9907e-07],
            [1.0255e-01, 1.2997e-05, 3.7469e-07],
            [1.0767e-01, 1.1656e-05, 3.4282e-07],
            [1.1306e-01, 1.0820e-05, 3.2299e-07],
            [1.1871e-01, 9.5831e-06, 3.0088e-07],
            [1.2465e-01, 8.9996e-06, 2.8102e-07],
            [1.3088e-01, 8.6498e-06, 2.7363e-07],
            [1.3742e-01, 7.9412e-06, 2.7158e-07],
            [1.4429e-01, 7.4042e-06, 2.6829e-07],
            [1.5151e-01, 6.8321e-06, 2.6877e-07],
            [1.5908e-01, 5.6809e-06, 2.6232e-07],
            [1.6704e-01, 5.6646e-06, 2.8607e-07],
            [1.7539e-01, 4.7762e-06, 2.7767e-07],
            [1.8416e-01, 4.1971e-06, 2.7353e-07],
            [1.9337e-01, 4.1815e-06, 2.9140e-07],
            [2.0304e-01, 3.2725e-06, 2.3160e-07],
            [2.1319e-01, 2.3244e-06, 4.2270e-07],
            [2.2385e-01, 2.2892e-06, 1.4970e-07],
            [2.3504e-01, 1.9198e-06, 8.1460e-07],
            [2.4679e-01, 1.4828e-06, 1.0840e-07],
            [2.5913e-01, 1.1450e-06, 4.7440e-07],
            [2.7209e-01, 1.3047e-06, 1.8840e-07],
            [2.8569e-01, 9.6081e-07, 3.9385e-07],
            [2.9998e-01, 8.2280e-07, 2.3955e-07],
            [3.1497e-01, 6.3219e-07, 3.5324e-07],
            [3.3072e-01, 6.1254e-07, 5.0846e-07],
            [3.4726e-01, 7.4092e-07, 2.2312e-07],
            [3.6462e-01, 6.3584e-07, 4.2866e-07],
            [3.8285e-01, 7.7287e-07, 2.9493e-07],
            [4.0200e-01, 9.4637e-07, 2.3347e-07],
            [4.2210e-01, 7.0576e-07, 3.3494e-07],
            [4.4320e-01, 7.8969e-07, 2.3371e-07],
            [4.6536e-01, 5.8374e-07, 3.2624e-07],
            [4.8863e-01, 5.6711e-07, 6.0419e-07],
            [5.1306e-01, 4.7782e-07, 3.4822e-07],
            [5.3871e-01, 6.3813e-07, 4.3279e-07],
            [5.6565e-01, 4.6186e-07, 1.8863e-07],
            [5.8877e-01, 5.6129e-07, 4.3960e-07],
        ]
    )

    problem.data.append(name="H-tail / D-head / ACMW", data=d13acmw20)
    problem.data.append(name="D-tail / H-head / D2O", data=d70d2o20)

    problem.contrasts.append(
        name="D-tail/H-Head/D2O",
        background="Background D2O",
        resolution="Resolution 1",
        scalefactor="Scalefactor 1",
        bulk_out="SLD D2O",
        bulk_in="SLD Air",
        data="D-tail / H-head / D2O",
    )

    problem.contrasts.append(
        name="H-tail/D-Head/ACMW",
        background="Background ACMW",
        resolution="Resolution 1",
        scalefactor="Scalefactor 1",
        bulk_out="SLD ACMW",
        bulk_in="SLD Air",
        data="D-tail / H-head / D2O",
    )

    problem.contrasts.set_fields(0, model=["Deuterated Tails", "Hydrogenated Heads"])
    problem.contrasts.set_fields(1, model=["Hydrogenated Tails", "Deuterated Heads"])

    problem.background_parameters.set_fields(0, fit=True)
    problem.background_parameters.set_fields(1, fit=True)
    problem.scalefactors.set_fields(0, fit=True)
    problem.bulk_out.set_fields(0, fit=True)

    return problem


if __name__ == "__main__":
    problem = two_contrast_example()
    controls = RAT.Controls()
    problem, results = RAT.run(problem, controls)

    RAT.plotting.plot_ref_sld(problem, results, True)
