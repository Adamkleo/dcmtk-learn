# DCMTK Steps for me


## Complete C-FIND with Results

**Goal:** Query a DICOM server and process all returned study information.
- How to iterate through the response list returned by `sendFINDRequest()`
- Each item in the list is a `DcmDataset*` - extract data from it
- Build a query dataset with appropriate search keys
- What does `DCM_QueryRetrieveLevel` need to be? (PATIENT/STUDY/SERIES/IMAGE)
- Which tags to include for search criteria vs which to request back

Check the required query keys for STUDY level:
- Look up what's mandatory in DICOM standard Part 4 (continue reading)
- Patient Name, Patient ID are commonly used
- Use wildcards ("*") for broad searches

**Display the results:**
- Print in table format
- Show: Patient Name, Patient ID, Study Date, Study Description, Number of Series
- Handle cases where tags might be empty


**Testing:**
- Query for all studies
- Query for specific patient name
- Query with date range
- Query that returns no results

---

## Simple Storage SCP
Since the goal is to have an SCP or SCU, I should probably start with the SCP.

Create a server that accepts DICOM C-STORE requests and saves files. Keep it simple.

Use:
- DcmSCP class, the server counterpart to DcmSCU
- How to configure it to listen on a port
- What presentation contexts to accept (storage SOP classes)
- How to handle incoming associations

Key methods to find:
- Setting up the network
- Adding presentation contexts for storage
- Listening for connections
- Processing received datasets
- Saving to disk

Potential checklist:
- [ ] Initialize DcmSCP with port and AE title
- [ ] Add storage presentation contexts (MR, CT, XRay, etc.)
- [ ] Start listening loop
- [ ] Handle incoming connections
- [ ] Save received datasets with proper filenames
- [ ] Return appropriate DICOM status codes
- [ ] Log all operations

Filename strategy:
Either use SOP Instance UID as file name or look at how dicom files are named in transfer and follow that that.
Maybe even add a directory structure based on study or patient

Maybe some error handling:
- write failures
- Invalid DICOM data
- Unsupported SOP classes 
- Association problems (better to handle in server or client? status code probably)

Testing: Use storescu command-line tool to send files to your SCP, Verify files are saved correctly, Test with different image types, Test error conditions

---

## Test Your SCP with Your SCU

Send files from my client to my server

Check if
- Files arrive intact (compare checksums)
- Metadata is preserved
- Your SCP handles multiple rapid sends (mayeb shouldnt worry about this for now)
- Error cases work (stop SCP, send from SCU , does it fail gracefully?)

There are always more things to test like full disk spcae, timing issues when starting up, etc
but i shouldnt worry about those for now, focus on the important

Very important: enable logs/log both sides to watch the dicom message exchange
and how the association works

---


BEYOND THIS POINT ARE THINGS I FOUND ONLINE, ADVANCED AND SPECIFIC, FOCUS ON THE PREVIOUS FIRST
## C-GET (Direct Retrieval)

**Goal:** Query and retrieve images in one operation directly to your client.

**Concept:**
- C-GET combines query and retrieve
- Server sends images directly to requesting client
- Client must act as storage SCP while sending C-GET

**What to figure out:**
- C-GET SOP Classes (StudyRootQueryRetrieveInformationModelGET, etc.)
- Your client needs dual role: send GET request AND receive STORE
- How to handle the response dataset
- How to receive and save incoming images

**Implementation approach:**
- Client sends C-GET with query keys
- Server responds with sub-operations (C-STORE requests back to client)
- Client must have storage SCP capability active
- Track number of completed/remaining/failed sub-operations

**Key differences from C-FIND:**
- Returns actual images, not just metadata
- Requires storage capability on client
- More complex status tracking

**Testing:**
- Retrieve single study
- Retrieve multiple studies
- Handle partial failures
- Track progress of multi-image retrieval

---

## Exercise 13: C-MOVE (Three-Party Transfer)

**Goal:** Request server to send images to a third destination.

**Concept:**
- Most complex: three parties involved
- You (client) tell Server A to send images to Destination B
- Destination must be a storage SCP
- Different from C-GET which sends directly to requester

**Parties involved:**
1. Move SCU (your client) - sends C-MOVE request
2. Move SCP (archive server) - has the images
3. Storage SCP (destination) - receives the images

**What to figure out:**
- C-MOVE SOP Classes
- How to specify destination AE title
- The server needs to know the destination's network location
- Configuration of destination table

**Setup challenge:**
- Need a Move SCP that knows about your storage destination
- `dcmqrscp` can do this with proper configuration
- Or use two machines/processes

**Implementation:**
- Build move request dataset (like C-GET)
- Specify destination AE title in the request
- The destination must be running and reachable
- Track sub-operation status

**Use cases:**
- PACS workflow: workstation requests archive to send to another workstation
- Backup operations
- Distribution to multiple destinations

---

## Exercise 14: Set Up Full Query/Retrieve SCP

**Goal:** Configure and run a complete Q/R server that handles FIND, MOVE, and GET.

**Use `dcmqrscp`:**
- Find example configuration files in DCMTK installation
- Understand the config file format
- Set up database of DICOM files

**Configuration items:**
- Network parameters (port, AE title)
- Storage directories
- Known destinations (for C-MOVE)
- Access control
- Supported SOP classes

**Database setup:**
- Organize DICOM files in directory structure
- Run indexing command to build database
- Understand how `dcmqrscp` tracks files

**Testing your server:**
- Query with findscu
- Retrieve with movescu
- Retrieve with getscu
- Store new files with storescu

**This ties together:**
- Everything you've built
- Complete PACS-like functionality
- Real-world server behavior

---

## Exercise 15: Modality Worklist (MWL)

**Goal:** Query worklist server for scheduled procedures.

**Concept:**
- Hospital scheduling system maintains procedure list
- Modalities query for "what should I scan next?"
- Pre-fills patient demographics, procedure details
- Reduces data entry errors

**MWL Query Keys:**
- Scheduled Procedure Step Sequence (required)
  - Scheduled Station AE Title
  - Scheduled Procedure Step Start Date/Time
  - Modality
- Patient demographics
- Requested Procedure ID

**What to figure out:**
- MWL SOP Class UID
- How to build nested sequence queries
- Required vs optional return keys
- Date/time range queries

**Return attributes:**
- Patient Name, ID, Birth Date, Sex
- Accession Number
- Study Instance UID (to use for resulting images)
- Procedure description
- Referring physician

**Implementation:**
- Build query with Scheduled Procedure Step Sequence
- Parse response sequences
- Display worklist items in user-friendly format
- Handle selecting a worklist item

**Testing:**
- Need MWL SCP (more rare than storage/query servers)
- Check if company has test MWL
- Or set up `wlmscpfs` from DCMTK

---

## Exercise 16: MPPS (Modality Performed Procedure Step)

**Goal:** Report procedure status to information system.

**Concept:**
- Modality reports "I'm starting this procedure"
- Updates during procedure
- Reports "I'm done, here are the images I created"
- Closes the loop in radiology workflow

**MPPS Operations:**
- N-CREATE: Start procedure (IN PROGRESS status)
- N-SET: Complete procedure (COMPLETED or DISCONTINUED status)

**What to figure out:**
- MPPS SOP Class
- Building N-CREATE request dataset
- Required attributes (many!)
- Referenced SOP sequences
- How to link to images created

**Required attributes:**
- Performed Procedure Step Status
- Modality
- Study Instance UID
- Patient demographics
- Performed Protocol Code Sequence
- Image references

**Implementation:**
1. Create MPPS with IN PROGRESS status
2. Store images (C-STORE)
3. Update MPPS with COMPLETED status and image references

**Status codes:**
- IN PROGRESS
- COMPLETED  
- DISCONTINUED

**This is the most complex networking operation.**

---

## Key Concepts Reference

### Association Lifecycle
```
1. Request Association
2. Negotiate Presentation Contexts
3. Perform DICOM Operations
4. Release Association (or Abort)
```

### Presentation Context Structure
- Abstract Syntax: The SOP Class (what operation)
- Transfer Syntax: How data is encoded
- Context ID: Agreed upon identifier

### Query Dataset Building
- Add search criteria with values
- Add return keys with empty values
- Some tags are required, some optional
- Sequences need special handling

## Useful info

### ALWAYS REMEMBER
- negotiate association after adding contexts
- check OFCondition return values
- dont assume tags always exist in responses
- not handling empty result sets
- correct transfer syntax for operation

### DICOM Status Codes
- 0x0000: Success
- 0x0001: Warning
- 0xA700: Out of resources
- 0xA900: Dataset doesn't match SOP Class
- 0xC000: Can't understand
- 0xFE00: Cancel

### Command-Line Testing Tools (Might need)
- `echoscu` - test connectivity
- `storescu` - send files
- `findscu` - query
- `movescu` - retrieve via C-MOVE
- `getscu` - retrieve via C-GET
- `storescp` - simple storage receiver
- `dcmqrscp` - full Q/R server

---

## When You Get Merge Code Access


## Progress Checklist

- [ ] C-FIND returning and parsing results
- [ ] Storage SCP receiving files
- [ ] SCU sending to own SCP successfully
- [ ] C-GET retrieving images
- [ ] C-MOVE working with three parties
- [ ] Full Q/R server configured and tested
- [ ] MWL query working
- [ ] MPPS create and set operations
- [ ] All operations with proper error handling
- [ ] Logging and debugging set up
- [ ] Merge-to-DCMTK mapping started

