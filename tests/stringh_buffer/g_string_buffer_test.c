#include <stdio.h>
#include <stdlib.h>

#include "../../gh_buffer.h"
#include "../../gh_string.h"

int main(){
		char* x = "001e#";
		unsigned char* y = (unsigned char*)x;
	
		printf("%x == %x\n\n", x[4], y[4]);
	
		g_str_t* string1 = string_init();
		g_str_t* string2 = string_init();
		g_str_t* string3 = string_init();
		g_str_t* string4 = string_init();
		g_str_t* string5 = string_init();
		g_str_t* string6 = string_init();
				
		g_buff_t* buffer1 = buffer_init();
		g_buff_t* buffer2 = buffer_init();
		g_buff_t* buffer3 = buffer_init();
		
		string_char(string1, "one");
		string_char(string2, "two");
		
		string_add(string2, "three");
		string_add(string2, "xxx");
		
		printf("Testing string_add: %s\n", string2->str);
		
		string_free(string2);
		string_clean(string2);
		
		string2 = string_init();		
		
		printf("Testing string_add: %s\n", string2->str);
		
		string_add(string2, "xxxxxxxxxxxxxxxxx");
		string_add(string2, "yyx");
			
		printf("Testing string_add: %s\n", string2->str);
	
		string_add(string4, "test");
				
		printf("String2: %s\n", string2->str);
		
		string_copy_bytes_stop_at_char(string5, string4, 0, 's');
		printf("String5: %s\n", string5->str);
		
		string_hexsign(string1);

		buffer_char(buffer1, "three");
		buffer_char(buffer2, "four");
		
		string_save_to_file(string1, "/home/wubuntu/ext10/Projects/git-server/git_handler/tests/string_buffer/file.txt");		
		
		string_copy(string1, string2);
		buffer_copy(buffer1, buffer2);
		
		string_concate(string1, string1);
		buffer_concate(buffer1, buffer1);
		
		buffer_to_string(string3, buffer2);	
		string_to_buffer(buffer2, string3);
			
		printf("String3 after conversion: %s\n", string3->str);	
		printf("String1 after concate: %s\n", string1->str);	
		
		string_append(string1, "-formating%c%s", 'G', "test");
		string_append(string4, "-formating");
		
		string_append_hexsign(string6, "-formating");
		
		printf("String1 hex sign after append: %s\n", string6->str);	
		printf("String4 after append: %s\n", string4->str);	
		
		string_append(string4, "three%cthree", '\0');
		string_append(string4, "three%cthree", '\0');
		string_append(string4, "three%cthree", '\0');

		printf("String1 with terminating char: %s\n", string4->str);
		string_debug(string4);
				
		buffer_clean(buffer1);
		buffer_clean(buffer2);
		buffer_clean(buffer3);
		string_clean(string1);
		string_clean(string2);
		string_clean(string3);
		string_clean(string4);
		string_clean(string5);
		string_clean(string6);
					
		return 0;
}
