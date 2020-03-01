This project is made in Eclipse, under Eclipse you can build the mMesh.

To make googletest: 

cd /libraries/googletest-release-1.10.0

perhaps... if it complains:

cmake -G "Unix Makefiles"

make -j4

you need google test archive files to be able to link the application.


Also..

Under Settings for the project in Eclipse, navigate to

C/C++ Build -> Tool chain editor and choose your own.

Then everything should be set...
