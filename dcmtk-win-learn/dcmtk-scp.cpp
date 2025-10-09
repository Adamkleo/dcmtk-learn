/* Server Class Provider (SCP) class
...
...
*/
#include <iostream>
#include <dcmtk/dcmnet/scp.h>
#include <dcmtk/dcmnet/dstorscp.h>



int main() {

	DcmStorageSCP storageSCP;

	storageSCP.setPort(104);
	storageSCP.setAETitle("MY_SCP");

		
	OFCondition presStatus;

	if (presStatus.bad()) {
		std::cout << "Failed to add verification SOP class!" << std::endl;
	}
	
	presStatus = storageSCP.addPresentationContext(UID_XRayAngiographicImageStorage, { UID_LittleEndianExplicitTransferSyntax }, ASC_SC_ROLE_SCUSCP);
	if (presStatus.bad()) {
		std::cout << "Failed to add xray SOP class!" << std::endl;
	}
	else {
		std::cout << "Added xray SOP class!" << std::endl;
	}


	storageSCP.setOutputDirectory("C:/Users/z005729y/adamdicomtest");
	storageSCP.setDirectoryGenerationMode(DcmStorageSCP::DGM_SeriesDate);
	storageSCP.setFilenameGenerationMode(DcmStorageSCP::FGM_SOPInstanceUID);
	storageSCP.setFilenameExtension(".dcm");
	storageSCP.setDatasetStorageMode(DcmStorageSCP::DGM_StoreToFile);
	// Load an association configuration file



	// Very important. listen will only return on error, there are ways to stop it using connectio timeout,
	// check the docs for more info, for now its enough for me to just listen
	std::cout << "Starting SCP..." << std::endl;
	OFCondition status = storageSCP.listen();
	if (status.bad()) {
		std::cerr << "Error starting SCP: " << status.text() << std::endl;
		return -1;
	}



	return 0;
}