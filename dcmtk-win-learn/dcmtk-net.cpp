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



int main(int argc, char* argv[]) {

	
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
	client.setPeerPort(104);
	client.setPeerAETitle("SHS1554382C");
	client.setAETitle("CLIENT");

	if ((client.negotiateAssociation()).good()) {
		std::cout << "Association negotiated successfully!" << std::endl;
	}
	else {
		std::cerr << "Failed to negotiate association: " << status.text() << std::endl;
		return -1;
	}

	const T_ASC_PresentationContextID presID = client.findPresentationContextID(UID_VerificationSOPClass, UID_LittleEndianExplicitTransferSyntax);
	OFCondition echoTest = client.sendECHORequest(presID);

	if (echoTest.good()) {
		std::cout << "C-ECHO request successful!" << std::endl;
	}
	else {
		std::cerr << "C-ECHO request failed: " << echoTest.text() << std::endl;
		return -1;
	}


	const T_ASC_PresentationContextID presIDXRay = client.findPresentationContextID(UID_XRayAngiographicImageStorage, UID_LittleEndianExplicitTransferSyntax);
	DcmFileFormat fileformat;
	OFCondition loadStatus = fileformat.loadFile(argv[1]);
	if (loadStatus.good()) {
		std::cout << "File loaded successfully!" << std::endl;
	}
	else {
		std::cerr << "Failed to load file: " << loadStatus.text() << std::endl;
		return -1;
	}
	DcmDataset* dataset = fileformat.getDataset();
	Uint16 response;
	OFCondition storeTest = client.sendSTORERequest(presIDXRay, argv[1], dataset, response);

	if (storeTest.good()) {
		std::cout << "C-STORE request successful! Response code: " << response << std::endl;
	}
	else {
		std::cerr << "C-STORE request failed: " << storeTest.text() << std::endl;
		return -1;
	}


	/*
	DcmDataset* qDataset = new DcmDataset;
	qDataset->putAndInsertString(DCM_QueryRetrieveLevel, "STUDY");
	qDataset->putAndInsertString(DCM_PatientName, "*");
	qDataset->putAndInsertString(DCM_PatientID, "");

	
	OFCondition stat = client.addPresentationContext(UID_FINDPatientRootQueryRetrieveInformationModel, { UID_LittleEndianExplicitTransferSyntax }, ASC_SC_ROLE_DEFAULT);
	if (stat.good()) {
		std::cout << "Find Patient Root Query Retrieve Information Model added." << std::endl;
	}
	else { std::cout << "Find Patient Root Query Retrieve Information Model failed." << std::endl; }
	client.negotiateAssociation();
	
	const T_ASC_PresentationContextID presIDFind = client.findPresentationContextID(UID_FINDPatientRootQueryRetrieveInformationModel, UID_LittleEndianExplicitTransferSyntax);
	
	status = client.sendFINDRequest(presIDFind, qDataset, NULL);
	if (status.good()) {
		std::cout << "C-FIND request successful!" << std::endl;
	}
	else {
		std::cerr << "C-FIND request failed: " << status.text() << std::endl;
		return -1;
	}
	*/

	client.closeAssociation(DCMSCU_RELEASE_ASSOCIATION);

	return 0;
}
