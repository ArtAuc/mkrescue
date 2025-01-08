# MKRescue

<img src="https://github.com/ArtAuc/mkrescue/blob/main/media/logo.png?raw=true" align="right"
     alt="MKRescue logo" width="100" height="100">

MKRescue is a software solution for managing the records of an animal shelter. It is designed
as a tool for care workers to track information about animal residents, 
as well as visitors and members.

## Records

MKRescue allows to create and modify multiple registries:
* Veterinary appointments and vaccines
* Memberships
* Animal entries and exits
* Boarding
* Patrons

## Statistics
Statistics about the shelter are easily accessible on the homepage. The number 
of animals currently present, current memberships and other metrics are displayed 
and can be interacted with

## Encryption

MKRescue uses [SimpleCrypt](https://wiki.qt.io/Simple_encryption_with_SimpleCrypt) to 
securely store the application's sensitive data. It allows the user to choose a password for 
encryption and decryption of the database.

## Network Synchronization

Online one-way synchronization of the database and prescriptions is possible thanks to the SMTP protocol 
and Google's API. Combined with the implemented encryption, it allows for safe storage of data.
