# Theory

@warning Deprecated documentation!

KFS [kazoo file system] is a file system that stores your data as a series of discrete kazoo signals

How it works:

KTL utilizes a single `.wav` file to store all of your data. Each byte of data
is stored as a series of kazoo signals. This `.wav` file is interpreted by `kfs`
to extract the data. The `.wav` file data contains all information about the
files within a kazoo encoded header.

## Discrete Kazoo Signals

In order to store data, you must have a set of discrete symbols. These symbols
are each represented by a single kazoo sound. Kazoo or [kazooid](http://kazoologist.org/What_Is.html)
signals are inherently noisy and can be difficult to distinguish.

`theory.ipynb` contains an FFT analysis of two kazoo signals, ultimately they
are very similar.

<!-- @todo list the unique features of a kazoo signal -->

