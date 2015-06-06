# Matrix Generators For Substitution Models

In the context of this practical we required a list of all (non-redundant) substitution models.
Basically a model is a string of 6 digits in the range 1 to 6 (or 0 to 5) implying constraints by choosing the same value at different positions in the string.
Further explanation can be found here: http://mbe.oxfordjournals.org/content/21/6/1123.full.pdf+html

## Usage

This repository currently ships two completely different algorithms for generating the model strings.
By calling `make` you generate a binary which is able to perform both variants chosen by command line arguments.
* `matrix-gen.out --bf` uses the brute-force algorithm variant
* `matrix-gen.out --ind` uses the inductive algorithm variant

## Brute-Force Approach

This algorithm uses a generate & filter approach to first generate the complete redundant model space and then filtering (normalizing) each model to include it in the result set.

## Inductive Approach

This algorithm stepwise derives new model strings from existing ones.
Here all generated models are per-construction normalized and non-redundant.

## Verification

Both algorithms were developed independently and generate both exactly the same models, which are also found in the literature.
