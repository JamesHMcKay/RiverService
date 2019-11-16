# RiverService
This is a web service designed to aggregate flow and metadata delivered in SOS format for New Zealand rivers.

For current testing flow data is only updated every 120 minutes, and any data older than three hours is discarded.

Check current status here: http://riverservice.herokuapp.com/

## Service contract

All requests are POST requests with header "Content-Type:application/json" and body sent as raw JSON.

* Get all available features that have flow and/or stage_height measurments

```javascript
{
	"action": "get_features",
	"filters": ["flow", "stage_height"],
}
```

* Request flow information for gauge with id = 15341

```javascript
{
    "action": "get_flows",
    "id": ["15341"]
}
```
## Deployment

Set up a lightsail instance and use the launch_script file included in this repository.

Under the networking tab enable HTTPS traffic.

Get a SSL certificate using:

sudo certbot --standalone -d openriverdata.com -d www.openriverdata.com certonly

This will put the certificates in the right place.

Add the files in the server_files directory into the $HOME directory of the lightsail instance.  Once the container has built, run the update_script.sh to pull the container and start it within the nginx container.
