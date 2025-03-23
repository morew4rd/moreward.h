# delete existing file and create a new one
rm -f	moreward.h
touch	moreward.h

# add include guard
printf "#ifndef _MOREWARD_H_\n"				    >>  moreward.h
printf "#define _MOREWARD_H_\n"				    >>  moreward.h

# add comment/guide at the top
printf "\n/*\n"								    >>  moreward.h
printf "\nLicense (MIT) is at the EOF .\n\n"	>>	moreward.h
cat docs/readme.md							    >>  moreward.h
printf "\n*/\n\n"							  	>>  moreward.h

# add header file (remove top 3 and last 1 line from each)
sed '1,3d;$d' src/mg.h			    			>>  moreward.h

# add implementation include guard
printf "\n\n/* IMPLEMENTATION */\n\n"	    	>>  moreward.h
printf "\n#ifdef MOREWARD_IMPL\n\n"	      	    >>  moreward.h

# add impl. header file (remove top 3 and last 1 line from each)
# sed '1,3d;$d' src/i.h			    			>>  moreward.h

# add implementation files (remove top 1 line from each
#                           which includes m.h)
sed '1,2d' src/mg.c						    >>  moreward.h
# sed '1,2d' src/alloc.c						    >>  moreward.h
# sed '1,2d' src/buffer.c		    			    >>  moreward.h
# sed '2d' src/mg_create_destroy.c                >>  moreward.h
# sed '1,2d' src/list.c	        			    >>  moreward.h
# sed '1,2d' src/dict.c	         			    >>  moreward.h
# sed '1,2d' src/strbuffer.c	   				    >>  moreward.h
# sed '1,2d' src/log.c	           			    >>  moreward.h

# add end implementation include guard
printf "\n\n#endif /* MOREWARD_IMPL */\n\n"	    >>  moreward.h

# add license text at the bottom
printf "/*\n\n"								    >>  moreward.h
cat license.txt							        >>  moreward.h
printf "\n*/\n\n"								>>  moreward.h

# add end include guard
printf "#endif /* _MOREWARD_H_ */\n"			>>  moreward.h
