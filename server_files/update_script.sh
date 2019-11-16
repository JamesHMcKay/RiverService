sudo docker-compose down
sudo docker login --username=jhmckay93 -p=riverservice1037
sudo docker pull jhmckay93/riverservice:latest
sudo docker system prune -f
sudo docker-compose up -d
