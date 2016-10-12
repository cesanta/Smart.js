// Code generated by clubbygen.
// GENERATED FILE DO NOT EDIT
// +build clubby_strict

package config

import (
	"bytes"
	"context"
	"encoding/json"
	"fmt"
	"sync"

	"cesanta.com/clubby"
	"cesanta.com/clubby/endpoint"
	"cesanta.com/clubby/frame"
	"cesanta.com/common/go/ourjson"
	"cesanta.com/common/go/ourtrace"
	"github.com/cesanta/errors"
	"golang.org/x/net/trace"

	"github.com/cesanta/ucl"
	"github.com/cesanta/validate-json/schema"
	"github.com/golang/glog"
)

var _ = bytes.MinRead
var _ = fmt.Errorf
var emptyMessage = ourjson.RawMessage{}
var _ = ourtrace.New
var _ = trace.New

const ServiceID = "http://mongoose-iot.com/fw/v1/Config"

type SaveArgs struct {
	Reboot *bool `json:"reboot,omitempty"`
}

type SetArgs struct {
	Config ourjson.RawMessage `json:"config,omitempty"`
}

type Service interface {
	Get(ctx context.Context) (ourjson.RawMessage, error)
	Save(ctx context.Context, args *SaveArgs) error
	Set(ctx context.Context, args *SetArgs) error
}

type Instance interface {
	Call(context.Context, string, *frame.Command) (*frame.Response, error)
	TraceCall(context.Context, string, *frame.Command) (context.Context, trace.Trace, func(*error))
}

type _validators struct {
	// This comment prevents gofmt from aligning types in the struct.
	GetResult *schema.Validator
	// This comment prevents gofmt from aligning types in the struct.
	SaveArgs *schema.Validator
	// This comment prevents gofmt from aligning types in the struct.
	SetArgs *schema.Validator
}

var (
	validators     *_validators
	validatorsOnce sync.Once
)

func initValidators() {
	validators = &_validators{}

	loader := schema.NewLoader()

	service, err := ucl.Parse(bytes.NewBuffer(_ServiceDefinition))
	if err != nil {
		panic(err)
	}
	// Patch up shortcuts to be proper schemas.
	for _, v := range service.(*ucl.Object).Find("methods").(*ucl.Object).Value {
		if s, ok := v.(*ucl.Object).Find("result").(*ucl.String); ok {
			for kk := range v.(*ucl.Object).Value {
				if kk.Value == "result" {
					v.(*ucl.Object).Value[kk] = &ucl.Object{
						Value: map[ucl.Key]ucl.Value{
							ucl.Key{Value: "type"}: s,
						},
					}
				}
			}
		}
		if v.(*ucl.Object).Find("args") == nil {
			continue
		}
		args := v.(*ucl.Object).Find("args").(*ucl.Object)
		for kk, vv := range args.Value {
			if s, ok := vv.(*ucl.String); ok {
				args.Value[kk] = &ucl.Object{
					Value: map[ucl.Key]ucl.Value{
						ucl.Key{Value: "type"}: s,
					},
				}
			}
		}
	}
	var s *ucl.Object
	_ = s // avoid unused var error
	validators.GetResult, err = schema.NewValidator(service.(*ucl.Object).Find("methods").(*ucl.Object).Find("Get").(*ucl.Object).Find("result"), loader)
	if err != nil {
		panic(err)
	}
	s = &ucl.Object{
		Value: map[ucl.Key]ucl.Value{
			ucl.Key{Value: "properties"}: service.(*ucl.Object).Find("methods").(*ucl.Object).Find("Save").(*ucl.Object).Find("args"),
			ucl.Key{Value: "type"}:       &ucl.String{Value: "object"},
		},
	}
	if req, found := service.(*ucl.Object).Find("methods").(*ucl.Object).Find("Save").(*ucl.Object).Lookup("required_args"); found {
		s.Value[ucl.Key{Value: "required"}] = req
	}
	validators.SaveArgs, err = schema.NewValidator(s, loader)
	if err != nil {
		panic(err)
	}
	s = &ucl.Object{
		Value: map[ucl.Key]ucl.Value{
			ucl.Key{Value: "properties"}: service.(*ucl.Object).Find("methods").(*ucl.Object).Find("Set").(*ucl.Object).Find("args"),
			ucl.Key{Value: "type"}:       &ucl.String{Value: "object"},
		},
	}
	if req, found := service.(*ucl.Object).Find("methods").(*ucl.Object).Find("Set").(*ucl.Object).Lookup("required_args"); found {
		s.Value[ucl.Key{Value: "required"}] = req
	}
	validators.SetArgs, err = schema.NewValidator(s, loader)
	if err != nil {
		panic(err)
	}
}

func NewClient(i Instance, addr string) Service {
	validatorsOnce.Do(initValidators)
	return &_Client{i: i, addr: addr}
}

type _Client struct {
	i    Instance
	addr string
}

func (c *_Client) Get(pctx context.Context) (res ourjson.RawMessage, err error) {
	cmd := &frame.Command{
		Cmd: "/v1/Config.Get",
	}
	ctx, tr, finish := c.i.TraceCall(pctx, c.addr, cmd)
	defer finish(&err)
	_ = tr
	resp, err := c.i.Call(ctx, c.addr, cmd)
	if err != nil {
		return ourjson.RawMessage{}, errors.Trace(err)
	}
	if resp.Status != 0 {
		return ourjson.RawMessage{}, errors.Trace(&endpoint.ErrorResponse{Status: resp.Status, Msg: resp.StatusMsg})
	}

	tr.LazyPrintf("res: %s", ourjson.LazyJSON(&resp))

	bb, err := resp.Response.MarshalJSON()
	if err != nil {
		glog.Errorf("Failed to marshal result as JSON: %+v", err)
	} else {
		rv, err := ucl.Parse(bytes.NewReader(bb))
		if err == nil {
			if err := validators.GetResult.Validate(rv); err != nil {
				glog.Warningf("Got invalid result for Get: %+v", err)
				return ourjson.RawMessage{}, errors.Annotatef(err, "invalid response for Get")
			}
		}
	}
	var r ourjson.RawMessage
	err = resp.Response.UnmarshalInto(&r)
	if err != nil {
		return ourjson.RawMessage{}, errors.Annotatef(err, "unmarshaling response")
	}
	return r, nil
}

func (c *_Client) Save(pctx context.Context, args *SaveArgs) (err error) {
	cmd := &frame.Command{
		Cmd: "/v1/Config.Save",
	}
	ctx, tr, finish := c.i.TraceCall(pctx, c.addr, cmd)
	defer finish(&err)
	_ = tr

	tr.LazyPrintf("args: %s", ourjson.LazyJSON(&args))
	cmd.Args = ourjson.DelayMarshaling(args)
	b, err := cmd.Args.MarshalJSON()
	if err != nil {
		glog.Errorf("Failed to marshal args as JSON: %+v", err)
	} else {
		v, err := ucl.Parse(bytes.NewReader(b))
		if err != nil {
			glog.Errorf("Failed to parse just serialized JSON value %q: %+v", string(b), err)
		} else {
			if err := validators.SaveArgs.Validate(v); err != nil {
				glog.Warningf("Sending invalid args for Save: %+v", err)
				return errors.Annotatef(err, "invalid args for Save")
			}
		}
	}
	resp, err := c.i.Call(ctx, c.addr, cmd)
	if err != nil {
		return errors.Trace(err)
	}
	if resp.Status != 0 {
		return errors.Trace(&endpoint.ErrorResponse{Status: resp.Status, Msg: resp.StatusMsg})
	}
	return nil
}

func (c *_Client) Set(pctx context.Context, args *SetArgs) (err error) {
	cmd := &frame.Command{
		Cmd: "/v1/Config.Set",
	}
	ctx, tr, finish := c.i.TraceCall(pctx, c.addr, cmd)
	defer finish(&err)
	_ = tr

	tr.LazyPrintf("args: %s", ourjson.LazyJSON(&args))
	cmd.Args = ourjson.DelayMarshaling(args)
	b, err := cmd.Args.MarshalJSON()
	if err != nil {
		glog.Errorf("Failed to marshal args as JSON: %+v", err)
	} else {
		v, err := ucl.Parse(bytes.NewReader(b))
		if err != nil {
			glog.Errorf("Failed to parse just serialized JSON value %q: %+v", string(b), err)
		} else {
			if err := validators.SetArgs.Validate(v); err != nil {
				glog.Warningf("Sending invalid args for Set: %+v", err)
				return errors.Annotatef(err, "invalid args for Set")
			}
		}
	}
	resp, err := c.i.Call(ctx, c.addr, cmd)
	if err != nil {
		return errors.Trace(err)
	}
	if resp.Status != 0 {
		return errors.Trace(&endpoint.ErrorResponse{Status: resp.Status, Msg: resp.StatusMsg})
	}
	return nil
}

func RegisterService(i *clubby.Instance, impl Service) error {
	validatorsOnce.Do(initValidators)
	s := &_Server{impl}
	i.RegisterCommandHandler("/v1/Config.Get", s.Get)
	i.RegisterCommandHandler("/v1/Config.Save", s.Save)
	i.RegisterCommandHandler("/v1/Config.Set", s.Set)
	i.RegisterService(ServiceID, _ServiceDefinition)
	return nil
}

type _Server struct {
	impl Service
}

func (s *_Server) Get(ctx context.Context, src string, cmd *frame.Command) (interface{}, error) {
	r, err := s.impl.Get(ctx)
	if err != nil {
		return nil, errors.Trace(err)
	}
	bb, err := json.Marshal(r)
	if err == nil {
		v, err := ucl.Parse(bytes.NewBuffer(bb))
		if err != nil {
			glog.Errorf("Failed to parse just serialized JSON value %q: %+v", string(bb), err)
		} else {
			if err := validators.GetResult.Validate(v); err != nil {
				glog.Warningf("Returned invalid response for Get: %+v", err)
				return nil, errors.Annotatef(err, "server generated invalid responce for Get")
			}
		}
	}
	return r, nil
}

func (s *_Server) Save(ctx context.Context, src string, cmd *frame.Command) (interface{}, error) {
	b, err := cmd.Args.MarshalJSON()
	if err != nil {
		glog.Errorf("Failed to marshal args as JSON: %+v", err)
	} else {
		if v, err := ucl.Parse(bytes.NewReader(b)); err != nil {
			glog.Errorf("Failed to parse valid JSON value %q: %+v", string(b), err)
		} else {
			if err := validators.SaveArgs.Validate(v); err != nil {
				glog.Warningf("Got invalid args for Save: %+v", err)
				return nil, errors.Annotatef(err, "invalid args for Save")
			}
		}
	}
	var args SaveArgs
	if len(cmd.Args) > 0 {
		if err := cmd.Args.UnmarshalInto(&args); err != nil {
			return nil, errors.Annotatef(err, "unmarshaling args")
		}
	}
	return nil, s.impl.Save(ctx, &args)
}

func (s *_Server) Set(ctx context.Context, src string, cmd *frame.Command) (interface{}, error) {
	b, err := cmd.Args.MarshalJSON()
	if err != nil {
		glog.Errorf("Failed to marshal args as JSON: %+v", err)
	} else {
		if v, err := ucl.Parse(bytes.NewReader(b)); err != nil {
			glog.Errorf("Failed to parse valid JSON value %q: %+v", string(b), err)
		} else {
			if err := validators.SetArgs.Validate(v); err != nil {
				glog.Warningf("Got invalid args for Set: %+v", err)
				return nil, errors.Annotatef(err, "invalid args for Set")
			}
		}
	}
	var args SetArgs
	if len(cmd.Args) > 0 {
		if err := cmd.Args.UnmarshalInto(&args); err != nil {
			return nil, errors.Annotatef(err, "unmarshaling args")
		}
	}
	return nil, s.impl.Set(ctx, &args)
}

var _ServiceDefinition = json.RawMessage([]byte(`{
  "methods": {
    "Get": {
      "doc": "Get device config",
      "result": {
        "keep_as_json": true
      }
    },
    "Save": {
      "args": {
        "reboot": {
          "doc": "If set to ` + "`" + `true` + "`" + `, the device will be rebooted after saving config. It\nis often desirable because it's the only way to apply saved config.\n",
          "type": "boolean"
        }
      },
      "doc": "Save device config"
    },
    "Set": {
      "args": {
        "config": {
          "keep_as_json": true
        }
      },
      "doc": "Set device config"
    }
  },
  "name": "/v1/Config",
  "namespace": "http://mongoose-iot.com/fw"
}`))
