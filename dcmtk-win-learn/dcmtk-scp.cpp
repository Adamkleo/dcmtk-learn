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
	
	presStatus = storageSCP.addPresentationContext(UID_XRayAngiographicImageStorage, { UID_LittleEndianExplicitTransferSyntax }, ASC_SC_ROLE_DEFAULT);
	if (presStatus.bad()) {
		std::cout << "Failed to add xray SOP class!" << std::endl;
	}
	else {
		std::cout << "Added xray SOP class!" << std::endl;
	}
	storageSCP.addPresentationContext(UID_CTImageStorage, { UID_LittleEndianExplicitTransferSyntax }, ASC_SC_ROLE_DEFAULT);
	storageSCP.addPresentationContext(UID_MRImageStorage, { UID_LittleEndianExplicitTransferSyntax }, ASC_SC_ROLE_DEFAULT);
	storageSCP.addPresentationContext(UID_EnhancedMRImageStorage, { UID_LittleEndianExplicitTransferSyntax }, ASC_SC_ROLE_DEFAULT);


	// check docs on these options
	storageSCP.setDirectoryGenerationMode(DcmStorageSCP::DGM_SeriesDate);
	storageSCP.setOutputDirectory("C:\\");
	storageSCP.setFilenameGenerationMode(DcmStorageSCP::FGM_SOPInstanceUID);
	storageSCP.setFilenameExtension(".dcm");
	storageSCP.setDatasetStorageMode(DcmStorageSCP::DGM_StoreToFile);
	// Load an association configuration file


	//	OFLog::configure(OFLogger::DEBUG_LOG_LEVEL);
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