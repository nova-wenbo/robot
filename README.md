"# robot" 
echo "# robot" >> README.md
git init
git add README.md
git commit -m "first commit"
git remote add origin https://github.com/nova-wenbo/robot.git
git push -u origin master

git push origin HEAD:refs/for/master
