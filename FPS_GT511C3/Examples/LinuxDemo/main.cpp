#include <iostream>
#include <limits>
#include <unistd.h>

#include <FPS_GT511C3.h>

using namespace std;

static const char g_name[]    = "Linux FPS Demo";
static const char g_version[] = "1.0";

void print_usage(const char* argv0)
{
	cout << "\n"
	     << "Usage: " << argv0 << " <com_device> (Must run as root)\n"
	     << "\n"
	;
}

bool enroll(FPS_GT511C3& fps)
{
	int  iret;
	bool bret;
	bool retval = false;
	int  id = 0;

	// Find open enroll id
	for (bool exists=true;; id++) {
		if (id == FPS_GT511C3::MAX_ID) {
			cout << "Database full\n";
			goto end;
		}
		exists = fps.CheckEnrolled(id);
		if (!exists) break;
	}

	fps.EnrollStart(id);
	fps.SetLED(true);

	// Enroll fingerprint, first read
	cout << "Press finger to Enroll #" << id << "\n";
	while(fps.IsPressFinger() == false) usleep(1000);
	bret = fps.CaptureFinger(true);
	if (!bret) {
		cout << "Failed to capture first finger\n";
		goto end;
	}
	fps.Enroll1(); 
	cout << "Remove finger\n";
	while(fps.IsPressFinger() == true)  usleep(1000);

	// Enroll fingerprint, second read
	cout << "Press same finger again\n";
	while(fps.IsPressFinger() == false) usleep(1000);
	bret = fps.CaptureFinger(true);
	if (!bret) {
		cout << "Failed to capture second finger\n";
		goto end;
	}
	fps.Enroll2(); 
	cout << "Remove finger\n";
	while(fps.IsPressFinger() == true)  usleep(1000);

	// Enroll fingerprint, third and final read
	cout << "Press same finger again\n";
	while(fps.IsPressFinger() == false) usleep(1000);
	bret = fps.CaptureFinger(true);
	if (!bret) {
		cout << "Failed to capture thrid finger\n";
		goto end;
	}
	iret = fps.Enroll3(); 
	if (iret) {
		cout << "Failed to enroll finger with error code " << iret << "\n";
		goto end;
	}

	cout << "Finger enrolled with id " << id << "\n";
	retval = true;

end:
	fps.SetLED(false);
	return retval;
}

bool verify(FPS_GT511C3& fps)
{
	int  id;
	bool bret;
	bool retval = false;

	fps.SetLED(true);
	cout << "Press finger to verify\n";
	while(fps.IsPressFinger() == false) usleep(1000);
	bret = fps.CaptureFinger(true);
	if (!bret) {
		cout << "Failed to capture finger\n";
		goto end;
	}
	id = fps.Identify1_N(); 
	if (id < 200) {
		cout << "Fingerprint found, id #" << id << "\n";
		retval = true;
	} else {
		cout << "Fingerprint not found\n";
	}

end:
	fps.SetLED(false);
	return retval;
}

int main(int argc, char** argv)
{
	int res;
	const char* device = NULL;

	cout << "Running " << g_name << " " << g_version << "...\n";

	if (argc > 1) {
		device = argv[1];
	}
	if (!device) {
		print_usage(argv[0]);
		return 0;
	}

	FPS_GT511C3 fps(device);
	res = fps.Open();
	if (res < 0) {
		return res;
	}

	for (bool quit=false; !quit;) {
		char key;
		bool bval;
		int ival;

		cout << "\nEnter a command. Enter '?' to see all options: ";
		cin >> key;
		switch (key) {
			case 'c':
				bval = fps.DeleteAll();
				if (bval) {
					cout << "Database cleared\n";
				} else {
					cout << "Failed to clear database\n";
				}
				break;
			case 'd':
				cin >> ival;
				bval = fps.DeleteID(ival);
				if (bval) {
					cout << "Cleared ";
				} else {
					cout << "Failed to clear ";
				}
				cout << "entry #" << ival << "\n";
				break;
			case 'e':
				enroll(fps);
				break;
			case 'v':
				verify(fps);
				break;
			case 'i':
				cout << "Not implemented\n"; // TODO
				break;
			case 'l':
				cin >> bval;
				fps.SetLED(bval);
				break;
			case 'm':
				cin >> bval;
				fps.SetDebug(bval);
				cout << "Debug set to " << bval;
				break;
			case 'n': {
				int count = fps.GetEnrollCount();
				cout << "Stored fingerprints: " << count << "\n";
				break;
			}
			case 'q':
				quit = true;
				break;
			case 'h':
			case '?':
				cout << "List of commands:\n"
				     << "  c         - Clear database\n"
				     << "  d <id>    - Delete selected fingerprint\n"
				     << "  e         - Enroll fingerprint in database\n"
				     << "  v         - Verify fingerprint is in database\n"
				     << "  i         - Get ID of fingerprint\n"
				     << "  l <0/1>   - Toggle LED backlight off/on\n"
				     << "  m <0/1>   - Set hardware debug messages off/on\n"
				     << "  n         - Show number of fingerprints in database\n"
				     << "  h, ?      - Display help\n"
				     << "  q         - Quit\n"
				;
				break;
			default:
				cout << "Invalid command!\n";
				break;
		}
	}

	fps.Close();
	cout << "Exiting " << g_name << " " << g_version << "\n";
	return 0;
}
