# delete existing files and create a new one
rm -f	mg.h
touch	mg.h
rm -f	mg.c
touch	mg.c

# add include guard
printf "#ifndef _MG_H\n"	    			    >>  mg.h
printf "#define _MG_H\n"	    			    >>  mg.h

# add comment/guide at the top
printf "\n/*\n\n"	\						    >>  mg.h
# printf "\nLicense (MIT) is at the EOF .\n\n"	>>	mg.h
# cat readme.md							    	>>  mg.h
printf "simple C99 library. 2 file version"     >>  mg.h
printf "\n*/\n\n"							  	>>  mg.h

# add header file (remove top 3 and last 1 line from each)
sed '1,3d;$d' src/m.h			    			>>  mg.h

# add end include guard
printf "#endif /* _MG_H */\n"			        >>  mg.h

# add implementation include guard
# printf "\n\n/* IMPLEMENTATION */\n\n"	    	>>  mg.c
# printf "\n#ifdef MOREWARD_IMPL\n\n"	      	    >>  mg.c

# add impl. header file (remove top 3 and last 1 line from each)
sed '1,3d;$d' src/i.h			    			>>  mg.c
printf '#include "mg.h"\n'                      >>  mg.c

# add implementation files (remove top 1 line from each
#                           which includes m.h)
sed '1,2d' src/alloc.c						    >>  mg.c
sed '1,2d' src/buffer.c		    			    >>  mg.c
# sed '2d' src/mg_create_destroy.c                >>  mg.c
sed '1,2d' src/list.c	        			    >>  mg.c
sed '1,2d' src/dict.c	         			    >>  mg.c
sed '1,2d' src/strbuffer.c	   				    >>  mg.c
sed '1,2d' src/log.c	           			    >>  mg.c

# add end implementation include guard
# printf "\n\n#endif /* MOREWARD_IMPL */\n\n"	    >>  mg.c

# add license text at the bottom
# printf "/*\n\n"								    >>  mg.c
printf "\n"     								    >>  mg.c
# cat license.txt							        >>  mg.c
