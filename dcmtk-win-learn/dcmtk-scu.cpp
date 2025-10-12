/*
Adam Kaawach - 9/7/2025
This file contains the code I wrote to learn how to use the DCMTK library.
It covers:
	- Opening and reading a .dcm file
	- Iterating over all tags and printing them
	- Accessing specific tags
	- Handling sequences
	- Accessing and analyzing pixel data
	- Modifying an existing dataset and saving it to a new file

Notes for me:
	- Most of what I've seen uses OFCondition to verify if an operation succeeded
	using good() or bad(). Try to follow the same pattern dcmtk does.
	- DCMFileFormat and DCMPixelSequence are both DCMSequenceOfItems
	which is a DcmElement which is DcmObject. This inheritance structure is
	reoccuring, remember it.
*/


#include <iostream>
#include <unordered_map>
#include <memory>
#include <dcmtk/dcmdata/dcdatset.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/ofstd/offile.h>
#include <dcmtk/dcmdata/dctk.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcpixel.h>
#include <dcmtk/dcmnet/scu.h>



int main() {


	std::cout << "Hello, DCMTK!" << std::endl;

	DcmSCU client;

	OFCondition status;

	status = client.addPresentationContext(UID_VerificationSOPClass, { UID_LittleEndianExplicitTransferSyntax }, ASC_SC_ROLE_DEFAULT);
	if (status.good()) {
		std::cout << "Verification SOP class added." << std::endl;
	}
	else { std::cout << "Verification SOP class failed." << std::endl; }

	// should check the status for this
	status = client.addPresentationContext(UID_XRayAngiographicImageStorage, { UID_LittleEndianExplicitTransferSyntax }, ASC_SC_ROLE_DEFAULT);

	if ((client.initNetwork()).good()) {
		std::cout << "Network initialized successfully!" << std::endl;
	}
	else {
		std::cerr << "Failed to initialize network: " << status.text() << std::endl;
		return -1;
	}

	client.setPeerHostName("localhost");
	client.setPeerPort(11112);
	client.setPeerAETitle("STORESCP");
	client.setAETitle("CLIENT");

	if ((client.negotiateAssociation()).good()) {
		std::cout << "Association negotiated successfully!" << std::endl;
	}
	else {
		std::cerr << "Failed to negotiate association: " << status.text() << std::endl;
		return -1;
	}

	T_ASC_PresentationContextID presID = client.findPresentationContextID(UID_VerificationSOPClass, UID_LittleEndianExplicitTransferSyntax, ASC_SC_ROLE_DEFAULT);
	OFCondition echoTest = client.sendECHORequest(presID);

	if (echoTest.good()) {
		std::cout << "C-ECHO request successful!" << std::endl;
	}
	else {
		std::cerr << "C-ECHO request failed: " << echoTest.text() << std::endl;
		return -1;
	}

	// Release the associatio
	client.releaseAssociation();

	return 0;
}
