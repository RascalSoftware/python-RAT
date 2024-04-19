Contributing to Python-RAT
==========================
Everyone is welcome to contribute to the Python-RAT project by either opening an issue (please check that the 
issue has not been reported already) or submitting a pull request.

Create Developer Environment
----------------------------
This project targets Python 3.9 or later. Install an appropriate version of Python and other dependencies

Then create a fork of the python-RAT repo, and clone the fork

    git clone --recurse-submodules https://github.com/<username>/python-RAT.git
    cd python-RAT

And finally create a separate branch to begin work

    git checkout -b new-feature

If there are updates to the C++ RAT submodule, run the following command to update the local branch

    git submodule update --remote

Once complete submit a [pull request](https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/proposing-changes-to-your-work-with-pull-requests/creating-a-pull-request-from-a-fork) via GitHub. 
Ensure to rebase your branch to include the latest changes on your branch and resolve possible merge conflicts.

Unit-testing and coverage
-------------------------
Python-RAT uses the **pytest** module for testing. Proper documentation and unit tests are highly recommended.

To install pytest use

    pip install pytest pytest-cov

Run the tests and generate a coverage report with

    pytest tests --cov=RAT

The coverage report can be saved to the directory htmlcov by running the tests with

    pytest tests --cov-report html --cov=RAT

For information on other coverage report formats, see https://pytest-cov.readthedocs.io/en/latest/reporting.html

Documentation
-------------
The documentation will be hosted on GitHub pages.

Style guidelines
----------------
* Docstrings should be written in the numpydoc format.
