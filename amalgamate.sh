# delete existing file and create a new one
rm -f	moreward.h
touch	moreward.h

# add include guard
printf "#ifndef _MOREWARD_H_\n"				    >>  moreward.h
printf "#define _MOREWARD_H_\n"				    >>  moreward.h

# add comment/guide at the top
printf "\n/*\n"								    >>  moreward.h
printf "\nLicense info is at the end.\n\n"		>>	moreward.h
cat guide.md							    	>>  moreward.h
printf "\n*/\n\n"							  	>>  moreward.h

# add header file (remove top 3 and last 1 line from each)
sed '1,3d;$d' src/m.h			    			>>  moreward.h

# add implementation include guard
printf "\n\n/* IMPLEMENTATION */\n\n"	    	>>  moreward.h
printf "\n#ifdef MOREWARD_H_IMPL\n\n"	    	>>  moreward.h

# add implementation files (remove top 1 line from each
#                           which includes m.h)
sed '1d' src/mg_alloc.c						    >>  moreward.h
sed '1d' src/mg_buffer.c					    >>  moreward.h
sed '1d' src/mg_list.c	    				    >>  moreward.h
sed '1d' src/mg_map.c	    				    >>  moreward.h
sed '1d' src/mg_string.c	   				    >>  moreward.h

# add end implementation include guard
printf "\n#endif /* MOREWARD_H_IMPL */\n\n"	    >>  moreward.h

# add license text at the bottom
printf "/*\n\n"								    >>  moreward.h
cat license.txt							        >>  moreward.h
printf "\n*/\n\n"								>>  moreward.h

# add end include guard
printf "#endif /* _MOREWARD_H_ */\n"			>>  moreward.h
