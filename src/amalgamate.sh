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

# add header file (remove top 3 and last 1 line)
sed '1,3d;$d' src/mg.h			    			>>  moreward.h

# add implementation include guard
printf "\n\n/* IMPLEMENTATION */\n\n"	    	>>  moreward.h
printf "\n#ifdef MOREWARD_IMPL\n\n"	      	    >>  moreward.h

# add implementation file (remove top 1 linw)
sed '1,2d' src/mg.c						        >>  moreward.h

# add implementation include guard close
printf "\n\n#endif /* MOREWARD_IMPL */\n\n"	    >>  moreward.h

# add license text at the bottom
printf "/*\n\n"								    >>  moreward.h
cat license.txt							        >>  moreward.h
printf "\n*/\n\n"								>>  moreward.h

# add end include guard
printf "#endif /* _MOREWARD_H_ */\n"			>>  moreward.h
