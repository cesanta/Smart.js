---
title: "Auth"
---

Auth service performs authentication and authorization of devices and commands. Can only be used by the cloud backend.

#### Authenticate
Authenticate returns true if `id` is verified to have valid credentials.

Arguments:
- `credentials`: Credentials presented by the entity. At least one of `psk` or `cert` must be present.
- `id`: ID of an entity to authenticate or a username.

Result `object`: success: `true` if credentials are valid, `false` otherwise. id: effective user id; useful when the authenticating via username
Request:
```json
{
  "v": 2,
  "src": "device_123",
  "id": 123,
  "method": "/v1/Auth.Authenticate",
  "args": {
    "credentials": "VALUE PLACEHOLDER",
    "id": "VALUE PLACEHOLDER"
  }
}

```

Response:
```json
{
  "v": 2,
  "src": "//api.mongoose-iot.com",
  "dst": "device_123",
  "id": 123,
  "result": "VALUE PLACEHOLDER"
}

```

#### ListToken
Returns the list of apps for which tokens were generated.


Result `array`: 
Request:
```json
{
  "v": 2,
  "src": "device_123",
  "id": 123,
  "method": "/v1/Auth.ListToken",
  "args": {}
}

```

Response:
```json
{
  "v": 2,
  "src": "//api.mongoose-iot.com",
  "dst": "device_123",
  "id": 123,
  "result": "VALUE PLACEHOLDER"
}

```

#### RevokeToken
Revokes auth token for the given app.

Arguments:
- `host`: Hostname of the app.

Request:
```json
{
  "v": 2,
  "src": "device_123",
  "id": 123,
  "method": "/v1/Auth.RevokeToken",
  "args": {
    "host": "VALUE PLACEHOLDER"
  }
}

```

Response:
```json
{
  "v": 2,
  "src": "//api.mongoose-iot.com",
  "dst": "device_123",
  "id": 123
}

```

#### DefineRoleOps
Defines the role<->operations mappings for a given
realm. Custom backends and firmwares can extend the Mongoose IoT API
but all operations defined by user code live in its own realm.
Devices and services are also associated with realms. This allows the
authorization backend to namespace custom methods and their associated roles.


Arguments:
- `operations`: 
- `realm`: Realm name.

Request:
```json
{
  "v": 2,
  "src": "device_123",
  "id": 123,
  "method": "/v1/Auth.DefineRoleOps",
  "args": {
    "operations": "VALUE PLACEHOLDER",
    "realm": "VALUE PLACEHOLDER"
  }
}

```

Response:
```json
{
  "v": 2,
  "src": "//api.mongoose-iot.com",
  "dst": "device_123",
  "id": 123
}

```

#### GetToken
Returns auth token for the given app.

Arguments:
- `host`: Hostname of the app.

Result `object`: 
Request:
```json
{
  "v": 2,
  "src": "device_123",
  "id": 123,
  "method": "/v1/Auth.GetToken",
  "args": {
    "host": "VALUE PLACEHOLDER"
  }
}

```

Response:
```json
{
  "v": 2,
  "src": "//api.mongoose-iot.com",
  "dst": "device_123",
  "id": 123,
  "result": "VALUE PLACEHOLDER"
}

```

#### GenerateToken
Returns auth token for the given app.

Arguments:
- `host`: Hostname of the app.

Result `object`: 
Request:
```json
{
  "v": 2,
  "src": "device_123",
  "id": 123,
  "method": "/v1/Auth.GenerateToken",
  "args": {
    "host": "VALUE PLACEHOLDER"
  }
}

```

Response:
```json
{
  "v": 2,
  "src": "//api.mongoose-iot.com",
  "dst": "device_123",
  "id": 123,
  "result": "VALUE PLACEHOLDER"
}

```

#### CreateRealm
Create a realm


Arguments:
- `realm`: Realm name.

Request:
```json
{
  "v": 2,
  "src": "device_123",
  "id": 123,
  "method": "/v1/Auth.CreateRealm",
  "args": {
    "realm": "VALUE PLACEHOLDER"
  }
}

```

Response:
```json
{
  "v": 2,
  "src": "//api.mongoose-iot.com",
  "dst": "device_123",
  "id": 123
}

```

#### SetPolicy
Give a subject one or more access roles towards an object.
E.g. granting (subject user foo, role writer, object device bar) will
allow the user foo to perform operations that require the writer role
on the device bar.
A subject can be a group account as well.


Arguments:
- `object`: Entity ID of the policy object.
- `roles`: List of role names.
Existing policy entries for roles not mentioned here will be removed.

- `subject`: Entity ID of the policy subject.

Request:
```json
{
  "v": 2,
  "src": "device_123",
  "id": 123,
  "method": "/v1/Auth.SetPolicy",
  "args": {
    "object": "VALUE PLACEHOLDER",
    "roles": "VALUE PLACEHOLDER",
    "subject": "VALUE PLACEHOLDER"
  }
}

```

Response:
```json
{
  "v": 2,
  "src": "//api.mongoose-iot.com",
  "dst": "device_123",
  "id": 123
}

```

#### AuthorizeCommand
AuthorizeCommand returns true if `src` is allowed to send a given command to `dst`.

Arguments:
- `src`: ID of the command sender.
- `dst`: ID of the command recipient.
- `cmd`: Command name.

Result `boolean`: `true` if the command is allowed, `false` otherwise.
Request:
```json
{
  "v": 2,
  "src": "device_123",
  "id": 123,
  "method": "/v1/Auth.AuthorizeCommand",
  "args": {
    "cmd": "VALUE PLACEHOLDER",
    "dst": "VALUE PLACEHOLDER",
    "src": "VALUE PLACEHOLDER"
  }
}

```

Response:
```json
{
  "v": 2,
  "src": "//api.mongoose-iot.com",
  "dst": "device_123",
  "id": 123,
  "result": "VALUE PLACEHOLDER"
}

```

#### AuthorizeOperation
Check whether the subject entity can perform a given operation
on an object entity.


Arguments:
- `operation`: Operation name.
- `object`: Entity ID of the object.
- `subject`: Entity ID of the subject.

Result `boolean`: 
Request:
```json
{
  "v": 2,
  "src": "device_123",
  "id": 123,
  "method": "/v1/Auth.AuthorizeOperation",
  "args": {
    "object": "VALUE PLACEHOLDER",
    "operation": "VALUE PLACEHOLDER",
    "subject": "VALUE PLACEHOLDER"
  }
}

```

Response:
```json
{
  "v": 2,
  "src": "//api.mongoose-iot.com",
  "dst": "device_123",
  "id": 123,
  "result": "VALUE PLACEHOLDER"
}

```

#### GetPolicy
Returns the policies for a given subject.


Arguments:
- `object`: Entity ID of the policy object.
- `subject`: Entity ID of the policy subject.

Result `array`: List of role names
Request:
```json
{
  "v": 2,
  "src": "device_123",
  "id": 123,
  "method": "/v1/Auth.GetPolicy",
  "args": {
    "object": "VALUE PLACEHOLDER",
    "subject": "VALUE PLACEHOLDER"
  }
}

```

Response:
```json
{
  "v": 2,
  "src": "//api.mongoose-iot.com",
  "dst": "device_123",
  "id": 123,
  "result": "VALUE PLACEHOLDER"
}

```


