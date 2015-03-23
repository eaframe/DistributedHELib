#git commands
add:
	git add $(FILE)

commit:
	git commit -am "$(MSG)"

push:
	git push origin master

pull:
	git pull origin master
	
sync:
	make commit MSG="$(MSG)"
	make push
