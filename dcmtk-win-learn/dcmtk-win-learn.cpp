#include <iostream>
#include <dcmtk/dcmdata/dcdatset.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/ofstd/offile.h>
#include <dcmtk/dcmdata/dctk.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcpixel.h>


void printAllTags(DcmDataset* dataset) {
	DcmObject* element = dataset->nextInContainer(nullptr);
	while (element != nullptr) {
		element->print(std::cout);
		element = dataset->nextInContainer(element);
	}
}

OFCondition getDatasetTag(DcmDataset* dataset, DcmTag tag, OFString& ofString) {
	OFCondition status = dataset->findAndGetOFString(tag, ofString);
	return status;
}

int numberOfPixels(DcmDataset* dataset) {
	Uint16 rows, cols;

	if (dataset->findAndGetUint16(DCM_Rows, rows).good() && dataset->findAndGetUint16(DCM_Columns, cols).good()) {
		return rows * cols;
	}
	return -1;
}

int main(int argc, char* argv[]) {

	if (argc != 2) {
		std::cerr << "Usage: ./main <dcm file>" << std::endl;
		return -1;
	}

	DcmFileFormat fileformat;
	OFCondition status = fileformat.loadFile(argv[1]);

	if (status.good()) {
		std::cout << "File loaded successfully!" << std::endl;

		DcmDataset* dataset = fileformat.getDataset();
		printAllTags(dataset);

		OFString patientName, studyDate, modality, refPhysAddr, refPhysName;

		OFCondition compressionStatus = dataset->chooseRepresentation(EXS_LittleEndianExplicit, nullptr);
		if (compressionStatus.good()) {
			std::cout << "Representation to Little Endian Explicit successful" << std::endl;
		}
		else {
			std::cout << "Representation to Little Endian Explicit failed" << std::endl;
			std::cout << compressionStatus.text() << std::endl;
		}


		if (getDatasetTag(dataset, DCM_PatientName, patientName).good()) {
			std::cout << "Patient before updating: " << patientName << std::endl;
		}
		else { std::cout << "Patient tag not found." << std::endl; }

		// dataset->putAndInsertString(DCM_PatientName, "Test^Patient");
		// After changing name in dataset
		if (getDatasetTag(dataset, DCM_PatientName, patientName).good()) {
			std::cout << "Patient after updating: " << patientName << std::endl;
		}
		else { std::cout << "Patient tag not found." << std::endl; }


		if (getDatasetTag(dataset, DCM_StudyDate, studyDate).good()) {
			std::cout << "Study Date: " << studyDate << std::endl;
		}
		else { std::cout << "Study date not found." << std::endl; }

		if (getDatasetTag(dataset, DCM_Modality, modality).good()) {
			std::cout << "Modality: " << modality << std::endl;
		}
		else { std::cout << "Modality tag not found." << std::endl; }

		if (getDatasetTag(dataset, DCM_ReferringPhysicianAddress, refPhysAddr).good()) {
			std::cout << "Referring Physician Address: " << refPhysAddr << std::endl;
		}
		else { std::cout << "Referring Physician Address not found." << std::endl; }

		if (getDatasetTag(dataset, DCM_ReferringPhysicianName, refPhysName).good()) {
			std::cout << "Referring Physician Name: " << refPhysName << std::endl;
		}
		else { std::cout << "Referring Physician Name not found." << std::endl; }




		int numPixels = numberOfPixels(dataset);
		std::cout << "Number of pixels: " << numPixels << std::endl;


		const Uint16* pixel;
		if (dataset->findAndGetUint16Array(DCM_PixelData, pixel).good()) {
			std::cout << "First pixel: " << *(pixel) << std::endl;

			double mean = 0;
			int min = *(pixel), max = *(pixel);
			for (int i = 0; i < numPixels; ++i) {
				const int currentPixel = *(pixel + i);
				mean += currentPixel;
				if (currentPixel < min) min = currentPixel;
				if (currentPixel > max) max = currentPixel;
			}
			mean /= numPixels;
			std::cout << "======= PIXEL DATA ======= " << std::endl;
			std::cout << "Mean: " << mean << std::endl;
			std::cout << "Min: " << min << std::endl;
			std::cout << "Max: " << max << std::endl;
		}

		//const OFFilename newFileName("MRHEAD.DCM"); 
		//fileformat.saveFile(newFileName);


	}
	else {
		std::cout << "Error loading file: " << status.text() << std::endl;
	}

	return 0;
}


