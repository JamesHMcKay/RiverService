# RiverService
This is web service designed to aggregate flow and metadata delivered in SOS format for New Zealand rivers.

For current testing flow data is only updated every 30 minutes, and any data older than three hours is discarded.

Check current status here: http://riverservice.herokuapp.com/

## Service contract

All requests are POST requests with header "Content-Type:application/json" and body sent as raw JSON.

* Get all available gauges with metadata and latest flow

```javascript
{
	"action": "get_features"
}
```

* Request flow information for gauge with id = 15341

```javascript
{
    "action": "get_flows",
    "id": ["15341"]
}
```
