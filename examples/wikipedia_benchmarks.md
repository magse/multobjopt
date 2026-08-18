# Wikipedia optimization benchmarks

The `WP` examples transcribe every named row displayed in Wikipedia's
[Test functions for optimization](https://en.wikipedia.org/wiki/Test_functions_for_optimization)
catalog into a separate `multobjopt` problem. The transcription was checked
against [revision 1369981485](https://en.wikipedia.org/w/index.php?title=Test_functions_for_optimization&oldid=1369981485)
on 2026-08-18. The numbering preserves that page's order across its
single-objective, constrained, and multi-objective sections. It is therefore
an inventory identifier, not a quality ranking.

These programs are executable examples and formula checks. Their deliberately
small, deterministic search budgets are useful for exercising the library, but
they are not a solver leaderboard or evidence that a method reliably finds a
global optimum. A serious algorithm comparison should use repeated independent
runs, equal evaluation budgets, suitable performance measures, and documented
hardware and compiler settings.

## Complete inventory

| Number | Source file | Kind | Parameters |
| --- | --- | --- | ---: |
| WP01 | `WP01_Rastrigin.cpp` | Single-objective | 5 |
| WP02 | `WP02_Ackley.cpp` | Single-objective | 2 |
| WP03 | `WP03_Sphere.cpp` | Single-objective | 5 |
| WP04 | `WP04_Rosenbrock.cpp` | Single-objective | 5 |
| WP05 | `WP05_Beale.cpp` | Single-objective | 2 |
| WP06 | `WP06_Goldstein_Price.cpp` | Single-objective | 2 |
| WP07 | `WP07_Booth.cpp` | Single-objective | 2 |
| WP08 | `WP08_Bukin_N6.cpp` | Single-objective | 2 |
| WP09 | `WP09_Matyas.cpp` | Single-objective | 2 |
| WP10 | `WP10_Levi_N13.cpp` | Single-objective | 2 |
| WP11 | `WP11_Griewank.cpp` | Single-objective | 5 |
| WP12 | `WP12_Himmelblau.cpp` | Single-objective | 2 |
| WP13 | `WP13_Three_Hump_Camel.cpp` | Single-objective | 2 |
| WP14 | `WP14_Easom.cpp` | Single-objective | 2 |
| WP15 | `WP15_Cross_In_Tray.cpp` | Single-objective | 2 |
| WP16 | `WP16_Eggholder.cpp` | Single-objective | 2 |
| WP17 | `WP17_Holder_Table.cpp` | Single-objective | 2 |
| WP18 | `WP18_McCormick.cpp` | Single-objective | 2 |
| WP19 | `WP19_Schaffer_N2.cpp` | Single-objective | 2 |
| WP20 | `WP20_Schaffer_N4.cpp` | Single-objective | 2 |
| WP21 | `WP21_Styblinski_Tang.cpp` | Single-objective | 5 |
| WP22 | `WP22_Shekel.cpp` | Single-objective | 4 |
| WP23 | `WP23_Rosenbrock_Disk.cpp` | Constrained single-objective | 2 |
| WP24 | `WP24_Mishra_Bird_Constrained.cpp` | Constrained single-objective | 2 |
| WP25 | `WP25_Townsend_Modified.cpp` | Constrained single-objective | 2 |
| WP26 | `WP26_Keane_Bump.cpp` | Constrained single-objective | 2 |
| WP27 | `WP27_Binh_Korn.cpp` | Constrained multi-objective | 2 |
| WP28 | `WP28_Chankong_Haimes.cpp` | Constrained multi-objective | 2 |
| WP29 | `WP29_Fonseca_Fleming.cpp` | Multi-objective | 3 |
| WP30 | `WP30_Test_Function_4.cpp` | Constrained multi-objective | 2 |
| WP31 | `WP31_Kursawe.cpp` | Multi-objective | 3 |
| WP32 | `WP32_Schaffer_N1_Multiobjective.cpp` | Multi-objective | 1 |
| WP33 | `WP33_Schaffer_N2_Multiobjective.cpp` | Multi-objective | 1 |
| WP34 | `WP34_Poloni.cpp` | Multi-objective | 2 |
| WP35 | `WP35_ZDT1.cpp` | Multi-objective | 25 |
| WP36 | `WP36_ZDT2.cpp` | Multi-objective | 25 |
| WP37 | `WP37_ZDT3.cpp` | Multi-objective | 25 |
| WP38 | `WP38_ZDT4.cpp` | Multi-objective | 10 |
| WP39 | `WP39_ZDT6.cpp` | Multi-objective | 10 |
| WP40 | `WP40_Osyczka_Kundu.cpp` | Constrained multi-objective | 6 |
| WP41 | `WP41_CTP1.cpp` | Constrained multi-objective | 2 |
| WP42 | `WP42_Constr_Ex.cpp` | Constrained multi-objective | 2 |
| WP43 | `WP43_Viennet.cpp` | Multi-objective | 2 |

## Transcription decisions

- `multobjopt` requires finite lower and upper parameter bounds. For Sphere,
  Rosenbrock, and Griewank, whose catalog domains are unbounded, the examples
  declare practical finite working boxes. The exact bounds are stated in each
  source file.
- Scalable single-objective formulas use five parameters so their summation and
  product structure remains visible without making the demonstrations slow.
- The catalog defines ZDT1, ZDT2, and ZDT3 with 30 parameters, while
  `multobjopt` deliberately supports at most 25. Their examples use 25 and the
  scalable normalization `9 / (n - 1)`; their objective shape is otherwise
  unchanged.
- The catalog's Shekel row gives the reciprocal-sum family but omits its
  coefficient tables and a complete domain. `WP22_Shekel.cpp` supplies the
  common four-parameter, ten-center coefficient set on `[0, 10]^4` and
  maximizes the positive reciprocal sum. This is equivalent to the conventional
  benchmark that minimizes its negative.
- Wikipedia writes several constraints as strict inequalities. A floating-point
  optimizer needs a closed, testable boundary, so the examples use the
  corresponding non-strict boundary. Every callback is translated to
  `restriction >= 0`, which is `multobjopt`'s feasibility convention.
- The ASCII filename `WP17_Holder_Table.cpp` avoids a platform-sensitive
  accented filename; its source comment retains the Hölder spelling.

`wp_example_support.hpp` contains only the common runner. Every mathematical
objective and restriction remains visible in the numbered `.cpp` file, making
each example easy to copy and adapt.
