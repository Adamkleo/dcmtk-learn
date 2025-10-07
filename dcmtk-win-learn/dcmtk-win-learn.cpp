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


/** Iterate over all tags in the data and print to standard out
 *  @param dataset - pointer to a dataset that will be iterated over
 */
static void printAllTags(DcmDataset* dataset) {
	DcmObject* element = dataset->nextInContainer(nullptr);
	while (element != nullptr) {
		element->print(std::cout);
		element = dataset->nextInContainer(element);
	}
}

/* Get a specific tag from the dataset
 *  @param dataset - pointer to a dataset that will be searched
 *  @param tag - the tag to search for
 *  @param ofString - reference to an OFString that will hold the value of the tag if found
 *  @return OFCondition - status of the operation (good if successful)
 *	Notes: This function is only meant for OFString tags. Other types like dcmtk int types, sequence items, 
 *	etc. will require different handling but since most things are ofstrings, it was enough for the example
*/
OFCondition getDatasetTag(DcmDataset* dataset, DcmTag tag, OFString& ofString) {
	OFCondition status = dataset->findAndGetOFString(tag, ofString);
	return status;
}


/* Calculate the number of pixels of the image in the dataset
 *  @param dataset - pointer to a dataset that will be searched
 *  @return int - number of pixels (rows * cols) or -1 if rows or cols not found
*/
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
	
	// Load the DICOM file from cmd line arg
	DcmFileFormat fileformat;
	OFCondition status = fileformat.loadFile(argv[1]);

	if (status.good()) {
		std::cout << "File loaded successfully!" << std::endl;
		
		// Get the dataset from the file
		DcmDataset* dataset = fileformat.getDataset();
		
		printAllTags(dataset);

		// OFStrings for the tags we want
		OFString patientName, studyDate, modality, refPhysAddr, refPhysName;

		// Map the attribute to its dcmtk tag and the ofstring we want to populate
		std::unordered_map<std::string, std::pair<DcmTagKey, std::reference_wrapper<OFString>>> tags = {
			{"Patient Name", {DCM_PatientName, std::ref(patientName)}},
			{"Study Date", {DCM_StudyDate, std::ref(studyDate)}},
			{"Test Modality", {DCM_Modality, std::ref(modality)}},
			{"Referring Physician Address", {DCM_ReferringPhysicianAddress, std::ref(refPhysAddr)}},
			{"Referring Physician Name", {DCM_ReferringPhysicianName, std::ref(refPhysName)}},
		};

		// Populate and print the OFString
		for (auto& [label, entry] : tags) {
			if (getDatasetTag(dataset, entry.first, entry.second.get()).good()) {
				std::cout << label << ": " << entry.second.get() << std::endl;
			}
			else {
				std::cout << label << " not found" << std::endl;
			}
		}

		// # UNCOMMENT NEXT LINE FOR CHANGING PATIENT NAME
		// Doesnt check OFCondition, probably should
		// dataset->putAndInsertString(DCM_PatientName, "Test^Patient");
		
		
		DcmSequenceOfItems* seq = nullptr;
		dataset->findAndGetSequence(DCM_AnatomicRegionSequence, seq);
		if (seq != nullptr) {
			std::cout << "Source Image Sequence found with " << seq->card() << " items." << std::endl;
		}
		else {
			std::cout << "Source Image Sequence not found." << std::endl;
		}

		if (seq == nullptr || seq->card() < 1) {
			std::cout << "No items in sequence." << std::endl;
		}
		else {
			int index = 0;
			DcmItem* seqItem = seq->getItem(index);
			std::cout << "Sequence item at index" << index << ": " << seqItem << std::endl;
			seqItem->print(std::cout);
		}
	
		int numPixels = numberOfPixels(dataset);
		std::cout << "Number of pixels: " << numPixels << std::endl;

		// Monochrome pixel is 2 bytes or 16 usngined bits.
		// We just need a pointer to the first and it will be treated as an array
		// Compute min, max, mean of pixels
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

		// #### UNCOMMENT NEXT LINE TO SAVE DICOM FILE TO A NEW FILE
		// saveFile() has flags if i want to replace the existing file, keep that in mind
		//const OFFilename newFileName("MRHEAD.DCM"); 
		//fileformat.saveFile(newFileName);


	}
	else {
		std::cout << "Error loading file: " << status.text() << std::endl;
	}

	return 0;
}


