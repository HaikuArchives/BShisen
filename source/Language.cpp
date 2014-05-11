//
// Multi-language support
//
//


#include <Directory.h>
#include <Roster.h>
#include <Application.h>
#include <Entry.h>
#include <Path.h>
#include <stdio.h>

void BuildLanguageList(void)
{
struct app_info info;
BEntry e;
BEntry e2;
BPath p;
BDirectory d;
char name[B_FILE_NAME_LENGTH];

	//if (be_roster->GetActiveAppInfo(&info) != B_OK) return;
	if (be_app->GetAppInfo(&info) != B_OK) return;
	e.SetTo(&info.ref, true);
	e.GetParent(&e2);
	e2.GetPath(&p);	
	p.Append("Languages");
	if (d.SetTo(p.Path()) != B_OK) return;
	//printf("%s\n",p.Path());


	while (d.GetNextEntry(&e) == B_OK)
	{
		e.GetName(name);
		printf("%s\n", name);
	}	
}